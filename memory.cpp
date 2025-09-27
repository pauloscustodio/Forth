//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "memory.h"
#include "vm.h"
#include <cstring>

Mem::Mem() {
    memset(data_, 0, sizeof(data_));
    bottom_ = 0;
    top_ = static_cast<int>(sizeof(data_));
}

uint Mem::addr(const char* ptr) const {
    uint addr = check_addr(static_cast<uint>(ptr - data_));
    return addr;
}

uint Mem::addr(const int* ptr) const {
    uint addr = check_addr(static_cast<uint>(reinterpret_cast<const char*>
                           (ptr) - data_));
    return addr;
}

char* Mem::char_ptr(uint addr, uint size) {
    addr = check_addr(addr, size);
    return data_ + addr;
}

int* Mem::int_ptr(uint addr, uint size) {
    if ((addr % CELL_SZ) != 0) {
        error(Error::AddressAlignmentException);
        return nullptr;
    }
    addr = check_addr(addr, size);
    return reinterpret_cast<int*>(data_ + addr);
}

double* Mem::float_ptr(uint addr, uint size) {
    if ((addr % CELL_SZ) != 0) {
        error(Error::AddressAlignmentException);
        return nullptr;
    }
    addr = check_addr(addr, size);
    return reinterpret_cast<double*>(data_ + addr);
}

int Mem::fetch(uint addr) {
    return *int_ptr(addr, CELL_SZ);
}

void Mem::store(uint addr, int value) {
    *int_ptr(addr, CELL_SZ) = value;
}

dint Mem::dfetch(uint addr) {
    int hi = fetch(addr);
    int lo = fetch(addr + CELL_SZ);
    return mk_dcell(hi, lo);
}

void Mem::dstore(uint addr, dint value) {
    store(addr, dcell_hi(value));
    store(addr + CELL_SZ, dcell_lo(value));
}

void Mem::fstore(uint addr, double value) {
    *float_ptr(addr) = value;
}

double Mem::ffetch(uint addr) {
    return *float_ptr(addr);
}

int Mem::cfetch(uint addr) {
    return static_cast<uchar>(data_[check_addr(addr, CHAR_SZ)]);
}

void Mem::cstore(uint addr, int value) {
    data_[check_addr(addr, CHAR_SZ)] = value;
}

void Mem::fill(uint addr, uint size, char c) {
    memset(char_ptr(addr), c, size);
}

void Mem::erase(uint addr, uint size) {
    fill(addr, size, 0);
}

void Mem::move(int src, int dst, uint size) {
    memmove(char_ptr(dst), char_ptr(src), size);
}

char* Mem::alloc_bottom(uint size) {
    size = aligned(size);
    if (bottom_ + size >= top_) {
        error(Error::DictionaryOverflow);
    }
    char* ret = char_ptr(bottom_);
    bottom_ += size;
    return ret;
}

char* Mem::alloc_top(uint size) {
    size = aligned(size);
    if (bottom_ + size >= top_) {
        error(Error::DictionaryOverflow);
    }
    top_ -= size;
    return char_ptr(top_);
}

int Mem::check_addr(uint addr, uint size) const {
    if (addr + size > static_cast<int>(sizeof(data_))) {
        error(Error::InvalidMemoryAddress);
        return 0;
    }
    else {
        return addr;
    }
}

//-----------------------------------------------------------------------------

void f_fill() {
    int c = pop();
    int n = pop();
    uint addr = pop();
    memset(mem_char_ptr(addr, n), c, n);
}

void f_erase() {
    int n = pop();
    uint addr = pop();
    memset(mem_char_ptr(addr, n), 0, n);
}

void f_move() {
    int n = pop();
    int dst = pop();
    int src = pop();
    memmove(mem_char_ptr(dst, n), mem_char_ptr(src, n), n);
}

//-----------------------------------------------------------------------------

// initialize the heap with a single large free block
void Heap::init() {
    size_ = vm.heap_hi_mem - vm.heap_lo_mem;
    pool_ = vm.heap_lo_mem;
    Block* free_block = reinterpret_cast<Block*>(mem_char_ptr(pool_));
    free_block->size = size_ - sizeof(Block);
    free_block->free = true;
    free_block->next = 0;
}

// allocate memory using first fit strategy
uint Heap::allocate(uint size) {
    size = aligned(size);

    Block* curr = reinterpret_cast<Block*>(mem_char_ptr(pool_));
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split block if there's enough space
            if (curr->size >= size + sizeof(Block) + DCELL_SZ) {
                // Split block
                char* new_block_addr =
                    reinterpret_cast<char*>(curr) + sizeof(Block) + size;
                Block* new_block = reinterpret_cast<Block*>(new_block_addr);
                new_block->size = curr->size - size - sizeof(Block);
                new_block->free = true;
                new_block->next = curr->next;
                curr->next = mem_addr(reinterpret_cast<char*>(new_block));
                curr->size = size;
            }
            curr->free = false;
            return mem_addr(reinterpret_cast<char*>(curr)) + sizeof(Block);
        }
        Block* next_block =
            curr->next ? reinterpret_cast<Block*>(mem_char_ptr(curr->next)) :
            nullptr;
        curr = next_block;
    }
    return 0; // no suitable block found
}

void Heap::free(uint ptr) {
    if (ptr == 0) {
        return;
    }

    char* block_addr = mem_char_ptr(ptr) - sizeof(Block);
    Block* block = reinterpret_cast<Block*>(block_addr);
    block->free = true;

    // Coalesce adjacent free blocks
    Block* curr = reinterpret_cast<Block*>(mem_char_ptr(pool_));
    while (curr) {
        Block* next_block =
            curr->next ? reinterpret_cast<Block*>(mem_char_ptr(curr->next)) :
            nullptr;
        if (curr->free && next_block && next_block->free) {
            curr->size += sizeof(Block) + next_block->size;
            curr->next = next_block->next;
        }
        else {
            curr = next_block;
        }
    }
}

uint Heap::resize(uint ptr, uint new_size) {
    new_size = aligned(new_size);
    if (ptr == 0) {
        return allocate(new_size);
    }

    char* block_addr = mem_char_ptr(ptr) - sizeof(Block);
    Block* block = reinterpret_cast<Block*>(block_addr);
    if (block->size >= new_size) {
        return ptr; // Current block is sufficient
    }

    // allocate a new block and copy data
    uint new_ptr = allocate(new_size);
    if (new_ptr) {
        memcpy(mem_char_ptr(new_ptr), mem_char_ptr(ptr), block->size);
        free(ptr);
    }

    return new_ptr;
}

//-----------------------------------------------------------------------------

void f_allocate() {
    uint size = pop();
    uint ptr = vm.heap.allocate(size);
    if (ptr) {
        push(ptr);
        push(0); // no error
    }
    else {
        push(0); // invalid address
        push(static_cast<int>(Error::AllocateException));
    }
}

void f_free() {
    uint ptr = pop();
    if (ptr != 0) {
        vm.heap.free(ptr);
        push(0); // no error
    }
    else {
        push(static_cast<int>(Error::FreeException));
    }
}

void f_resize() {
    uint new_size = pop();
    uint ptr = pop();
    uint new_ptr = vm.heap.resize(ptr, new_size);
    if (new_ptr) {
        push(new_ptr);
        push(0); // no error
    }
    else {
        push(ptr); // did not resize
        push(static_cast<int>(Error::ResizeException));
    }
}

