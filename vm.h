//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "core_mem.h"
#include "core_stack.h"
#include "types.h"
using namespace std;

struct VM {
	VM();

	// memory
	Mem mem;

	// character buffers
	Pad* pad;
	Tib* tib;
	Wordbuf* wordbuf;

	// stacks
	Stack* rstack;			// return stack
	Stack* stack;			// data stack

	// input
	void f_s_quote();
	void f_type();
	void f_environment_q();
};

extern VM vm;

// stacks
static inline void push(int value) { vm.stack->push(value); }
static inline int pop() { return vm.stack->pop(); }
static inline int peek(int depth = 0) { return vm.stack->peek(depth); }

static inline void rpush(int value) { vm.rstack->push(value); }
static inline int rpop() { return vm.rstack->pop(); }
static inline int rpeek(int depth = 0) { return vm.rstack->peek(depth); }

// variables
static inline void f_pad() { push(vm.mem.addr(vm.pad->pad())); }

// memory access
static inline void f_store() { int addr = pop(); int v = pop(); *vm.mem.int_ptr(addr) = v; }
static inline void f_fetch() { int addr = pop(); push(*vm.mem.int_ptr(addr)); }
static inline void f_c_store() { int addr = pop(); int v = pop(); *vm.mem.char_ptr(addr) = v; }
static inline void f_c_fetch() { int addr = pop(); push(*vm.mem.char_ptr(addr)); }

// WORD
char* c_word(char delimiter = BL);
void f_word();

// WORDS
void f_words();

// strings
static inline void f_count(void) { int addr = pop(); int len = *vm.mem.char_ptr(addr++); push(addr); push(len); }

