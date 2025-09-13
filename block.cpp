//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "block.h"
#include "errors.h"
#include "parser.h"
#include "vm.h"
#include <cassert>
#include <cstring>

void Block::init(uint index, int blk) {
    this->index = index;
    this->blk = blk;
    this->dirty = false;
    memset(data(), BL, BLOCK_SZ);
}

char* Block::data() const {
    assert(index < NUM_BLK_BUFFERS);
    return vm.block_data + index * BLOCK_SZ;
}

//-----------------------------------------------------------------------------

void Blocks::init() {
    f_empty_buffers();
}

void Blocks::deinit() {
    if (block_file_id_ != 0) {
        Error error_code = Error::None;
        vm.files.close(block_file_id_, error_code);
        if (error_code != Error::None) {
            error(error_code, BLOCKS_FILE);
        }
    }
}

int Blocks::num_blocks() {
    uint file_id = block_file_id();
    Error error_code = Error::None;
    udint size = vm.files.size(file_id, error_code);
    if (error_code != Error::None) {
        error(error_code, BLOCKS_FILE);
    }

    int num_blocks = static_cast<int>(size / BLOCK_SZ);
    return num_blocks;
}

Block* Blocks::f_block(int blk) {
    if (blk < 1) {
        error(Error::InvalidBlockNumber);
    }

    int index = find_buffer_index(blk);     // already exists, do not init
    if (index < 0) {
        index = find_first_unused();
        read_block(index, blk);
    }
    if (index < 0) {
        index = find_first_not_dirty();
        read_block(index, blk);
    }
    if (index < 0) {
        index = (last_block_ + 1) % NUM_BLK_BUFFERS;;
        flush_block(index);
        read_block(index, blk);
    }

    last_block_ = index;
    return &blocks_[last_block_];
}

void Blocks::f_empty_buffers() {
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        blocks_[i].init(i, 0);
    }
}

void Blocks::f_save_buffers() {
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        flush_block(i);
    }
}

void Blocks::f_flush() {
    f_save_buffers();
    f_empty_buffers();
}

void Blocks::f_list(int blk) {
    if (blk < 1) {
        error(Error::InvalidBlockNumber);
    }

    vm.user->SCR = blk;
    Block* block = f_block(blk);
    char* block_data = block->data();

    int save_base = vm.user->BASE;
    vm.user->BASE = 10;

    std::cout << std::endl << "Block ";
    print_number(blk);
    std::cout << std::endl;
    for (int row = 0; row < BLOCK_ROWS; ++row) {
        print_number(row + 1, 2);
        std::cout << BL;
        for (int col = 0; col < BLOCK_COLS; ++col) {
            char c = block_data[row * BLOCK_COLS + col];
            if (is_print(c)) {
                std::cout << c;
            }
            else {
                std::cout << "?";
            }
        }
        std::cout << std::endl;
    }

    vm.user->BASE = save_base;
}

void Blocks::f_load(int blk) {
    if (blk < 1) {
        error(Error::InvalidBlockNumber);
    }

    Block* block = f_block(blk);

    // save input context
    vm.input.save_input();

    // parse string
    vm.input.set_block(block);
    f_execute(xtINTERPRET);

    // restore input context
    vm.input.restore_input();
}

void Blocks::f_thru(int first, int last) {
    for (int i = first; i <= last; ++i) {
        f_load(i);
    }
}

void Blocks::f_update() {
    blocks_[last_block_].dirty = true;
}

uint Blocks::block_file_id() {
    if (block_file_id_ == 0) {
        block_file_id_ = vm.files.open_or_create(BLOCKS_FILE);

        if (block_file_id_ == 0) {
            error(Error::OpenFileException, BLOCKS_FILE);
        }
    }

    return block_file_id_;
}

bool Blocks::seek_block(int blk) {
    if (blk < 0) {
        error(Error::InvalidBlockNumber);
    }

    uint file_id = block_file_id();
    Error error_code = Error::None;
    std::streampos fpos = blk * BLOCK_SZ;
    vm.files.seek(file_id, fpos, error_code);
    if (error_code != Error::None) {
        error(error_code, BLOCKS_FILE);
    }

    // verify actual position
    std::streampos actual_pos = vm.files.tell(file_id, error_code);
    if (error_code != Error::None) {
        error(error_code, BLOCKS_FILE);
    }

    if (actual_pos != fpos) {
        return false;    // seek landed elsewhere (can happen on some devices)
    }

    return true;    // seek successful
}

int Blocks::find_buffer_index(int blk) const {
    if (blk < 0) {
        error(Error::InvalidBlockNumber);
    }

    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        if (blocks_[i].blk == blk) {
            return i;
        }
    }
    return -1;
}

int Blocks::find_first_unused() const {
    int j = last_block_;
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        j = (j + 1) % NUM_BLK_BUFFERS;
        if (blocks_[j].blk == 0) {
            return j;
        }
    }
    return -1;
}

int Blocks::find_first_not_dirty() const {
    int j = last_block_;
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        j = (j + 1) % NUM_BLK_BUFFERS;
        if (blocks_[j].blk != 0 && !blocks_[j].dirty) {
            return j;
        }
    }
    return -1;
}

void Blocks::flush_block(int index) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);

    if (blocks_[index].blk > 0) {
        if (blocks_[index].dirty) {
            write_block(index);
        }
        blocks_[index].dirty = false;
    }
}

bool Blocks::read_block(int index, int blk) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);
    if (blk < 0) {
        error(Error::InvalidBlockNumber);
    }

    blocks_[index].init(index, blk); // clear with blanks

    if (!seek_block(blk)) {
        return false;    // seek failed
    }

    uint file_id = block_file_id();
    Error error_code = Error::None;
    uint num_read = vm.files.read_bytes(file_id,
                                        blocks_[index].data(), BLOCK_SZ,
                                        error_code);
    if (error_code != Error::None) {
        error(error_code, BLOCKS_FILE);
    }

    if (num_read != BLOCK_SZ) {
        return false;    // read failed
    }
    else {
        return true;
    }
}

bool Blocks::write_block(int index) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);

    int blk = blocks_[index].blk;
    if (blk < 0) {
        error(Error::InvalidBlockNumber);
    }

    if (!seek_block(blk)) {
        return false;    // seek failed
    }

    uint file_id = block_file_id();
    Error error_code = Error::None;
    vm.files.write_bytes(file_id,
                         blocks_[index].data(), BLOCK_SZ,
                         error_code);
    if (error_code != Error::None) {
        error(error_code, BLOCKS_FILE);
    }

    return true;
}

void f_block() {
    int blk = pop();
    Block* block = vm.blocks.f_block(blk);
    const char* buffer = block->data();
    push(mem_addr(buffer));
}

void f_save_buffers() {
    vm.blocks.f_save_buffers();
}

void f_empty_buffers() {
    vm.blocks.f_empty_buffers();
}

void f_flush() {
    vm.blocks.f_flush();
}

void f_load() {
    int blk = pop();
    vm.blocks.f_load(blk);
}

void f_update() {
    vm.blocks.f_update();
}

void f_list() {
    int blk = pop();
    vm.blocks.f_list(blk);
}

void f_thru() {
    int last = pop();
    int first = pop();
    vm.blocks.f_thru(first, last);
}
