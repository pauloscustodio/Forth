//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "facility.h"
#include "forth.h"
#include "kbd_input.h"
#include "str.h"
#include "vm.h"

VM vm;

VM::VM() {
    // bottom of memory
    wordbuf_data = mem.alloc_bottom(WORDBUF_SZ);
    wordbuf.init();

    pad_data = mem.alloc_bottom(PAD_SZ);
    pad.init();

    number_output_data = mem.alloc_bottom(NUMBER_OUTPUT_SZ);
    number_output.init();

    tib_data = mem.alloc_bottom(TIB_SZ);
    input.init();

    block_data = mem.alloc_bottom(NUM_BLK_BUFFERS * BLOCK_SZ);
    blocks.init();

    // user variables
    user = reinterpret_cast<User*>(mem.alloc_bottom(sizeof(User)));
    user->init();

    // split the rest in two halves - dictionary and heap
    uint bottom = mem.addr(mem.alloc_bottom(0));
    uint top = mem.addr(mem.alloc_top(0));
    uint mid_mem = aligned((bottom + top) / 2);

    dict_lo_mem = bottom;
    dict_hi_mem = mid_mem;
    heap_lo_mem = mid_mem;
    heap_hi_mem = top;

    // initilize dictionary and heap
    dict.init();
    heap.init();

    // reinit wordbuf to get predictable results in tests
    wordbuf.init();

    init_console_output();
    init_console_input();
}

VM::~VM() {
    blocks.deinit();
}

// pointer - address conversion
uint mem_addr(const char* ptr) {
    return vm.mem.addr(ptr);
}

uint mem_addr(const int* ptr) {
    return vm.mem.addr(ptr);
}

uint mem_addr(const uint* ptr) {
    return vm.mem.addr(reinterpret_cast<const int*>(ptr));
}

uint mem_addr(const CString* ptr) {
    return vm.mem.addr(reinterpret_cast<const char*>(ptr));
}

char* mem_char_ptr(uint addr, uint size) {
    return vm.mem.char_ptr(addr, size);
}

int* mem_int_ptr(uint addr, uint size) {
    return vm.mem.int_ptr(addr, size);
}

// access memory
int fetch(uint addr) {
    return vm.mem.fetch(addr);
}

void store(uint addr, int value) {
    vm.mem.store(addr, value);
}

dint dfetch(uint addr) {
    return vm.mem.dfetch(addr);
}

void dstore(uint addr, dint value) {
    vm.mem.dstore(addr, value);
}

double ffetch(uint addr) {
    return vm.mem.ffetch(addr);
}

void fstore(uint addr, double value) {
    vm.mem.fstore(addr, value);
}

int cfetch(uint addr) {
    return vm.mem.cfetch(addr);
}

void cstore(uint addr, int value) {
    vm.mem.cstore(addr, value);
}

// allot dictionary space
void ccomma(int value) {
    vm.dict.ccomma(value);
}

void comma(int value) {
    vm.dict.comma(value);
}

void dcomma(dint value) {
    vm.dict.dcomma(value);
}

void fcomma(double value) {
    vm.dict.fcomma(value);
}

void align() {
    vm.dict.align();
}

// stacks
void push(int value) {
    vm.stack.push(value);
}

int pop() {
    return vm.stack.pop();
}

int peek(int depth) {
    return vm.stack.peek(depth);
}

int depth() {
    return vm.stack.depth();
}

void roll(int depth) {
    vm.stack.roll(depth);
}

void dpush(dint value) {
    push(dcell_lo(value));
    push(dcell_hi(value));
}

dint dpop() {
    int hi = pop();
    int lo = pop();
    return mk_dcell(hi, lo);
}

dint dpeek(int depth) {
    int hi = vm.stack.peek(2 * depth);
    int lo = vm.stack.peek(2 * depth + 1);
    return mk_dcell(hi, lo);
}

void r_push(int value) {
    vm.r_stack.push(value);
}

int r_pop() {
    return vm.r_stack.pop();
}

int r_peek(int depth) {
    return vm.r_stack.peek(depth);
}

int r_depth() {
    return vm.r_stack.depth();
}

void r_dpush(dint value) {
    r_push(dcell_lo(value));
    r_push(dcell_hi(value));
}

dint r_dpop() {
    int hi = r_pop();
    int lo = r_pop();
    return mk_dcell(hi, lo);
}

dint r_dpeek(int depth) {
    int hi = vm.r_stack.peek(2 * depth);
    int lo = vm.r_stack.peek(2 * depth + 1);
    return mk_dcell(hi, lo);
}

void cs_dpush(dint pos_addr) {
    vm.cs_stack.push(dcell_lo(pos_addr));
    vm.cs_stack.push(dcell_hi(pos_addr));
}

dint cs_dpop() {
    int hi = vm.cs_stack.pop();
    int lo = vm.cs_stack.pop();
    return mk_dcell(hi, lo);
}

dint cs_dpeek(int depth) {
    int hi = vm.cs_stack.peek(2 * depth);
    int lo = vm.cs_stack.peek(2 * depth + 1);
    return mk_dcell(hi, lo);
}

int cs_ddepth() {
    return vm.cs_stack.depth() / 2;
}

void fpush(double value) {
    vm.f_stack.push(value);
}

double fpop() {
    return vm.f_stack.pop();
}

double fpeek(int depth) {
    return vm.f_stack.peek(depth);
}

int fdepth() {
    return vm.f_stack.depth();
}
