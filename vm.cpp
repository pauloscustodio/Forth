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

    blocks = reinterpret_cast<Blocks*>(mem.alloc_bottom(sizeof(Blocks)));
    blocks->init();

    // user variables
    user = reinterpret_cast<User*>(mem.alloc_bottom(sizeof(User)));
    user->init();

    // top of memory
    r_stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
    r_stack->init('R',
                  Error::ReturnStackUnderflow, Error::ReturnStackOverflow);

    cs_stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
    cs_stack->init('C',
                   Error::ControlFlowStackUnderflow, Error::ControlFlowStackOverflow);

    except_stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
    except_stack->init('E',
                       Error::ExceptionStackUnderflow, Error::ExceptionStackOverflow);

    stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
    stack->init('\0',
                Error::StackUnderflow, Error::StackOverflow);

    // use the remaing as dictionary space
    dict = reinterpret_cast<Dict*>(mem.alloc_bottom(sizeof(Dict)));
    int start_dict = mem.addr(mem.alloc_bottom(0));
    int end_dict = mem.addr(mem.alloc_top(0));
    dict->init(start_dict, end_dict);

    // reinit wordbuf to get predictable results in tests
    wordbuf.init();

    init_console_output();
    init_console_input();
    files = new Files();
}

VM::~VM() {
    blocks->deinit();
    delete files;
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

int cfetch(uint addr) {
    return vm.mem.cfetch(addr);
}

void cstore(uint addr, int value) {
    vm.mem.cstore(addr, value);
}

// allot dictionary space
void ccomma(int value) {
    vm.dict->ccomma(value);
}

void comma(int value) {
    vm.dict->comma(value);
}

void dcomma(dint value) {
    vm.dict->dcomma(value);
}

void align() {
    vm.dict->align();
}

// stacks
void push(int value) {
    vm.stack->push(value);
}

int pop() {
    return vm.stack->pop();
}

int peek(int depth) {
    return vm.stack->peek(depth);
}

int depth() {
    return vm.stack->depth();
}

void roll(int depth) {
    vm.stack->roll(depth);
}

void dpush(dint value) {
    vm.stack->dpush(value);
}

dint dpop() {
    return vm.stack->dpop();
}

dint dpeek(int depth) {
    return vm.stack->dpeek(depth);
}

void r_push(int value) {
    vm.r_stack->push(value);
}

int r_pop() {
    return vm.r_stack->pop();
}

int r_peek(int depth) {
    return vm.r_stack->peek(depth);
}

int r_depth() {
    return vm.r_stack->depth();
}

void r_dpush(dint value) {
    vm.r_stack->dpush(value);
}

dint r_dpop() {
    return vm.r_stack->dpop();
}

dint r_dpeek(int depth) {
    return vm.r_stack->dpeek(depth);
}

void cs_dpush(dint pos_addr) {
    vm.cs_stack->dpush(pos_addr);
}

dint cs_dpop() {
    return vm.cs_stack->dpop();
}

dint cs_dpeek(int depth) {
    return vm.cs_stack->dpeek(depth);
}

int cs_ddepth() {
    return vm.cs_stack->depth() / 2;
}
