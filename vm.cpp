//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "vm.h"
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

VM vm;

//-----------------------------------------------------------------------------

void Pad::init() {
    memset(m_pad, BL, sizeof(m_pad));
}

//-----------------------------------------------------------------------------

VM::VM() {
	// bottom of memory
	wordbuf = reinterpret_cast<Wordbuf*>(mem.alloc_bot(sizeof(Wordbuf)));
	wordbuf->init();
	pad = reinterpret_cast<Pad*>(mem.alloc_bot(sizeof(Pad)));
	pad->init();
	number_output = reinterpret_cast<NumberOutput*>(mem.alloc_bot(sizeof(NumberOutput)));
	number_output->init();
	input = reinterpret_cast<Input*>(mem.alloc_bot(sizeof(Input)));
	input->init();

	// user variables
	user = reinterpret_cast<User*>(mem.alloc_bot(sizeof(User)));
	user->init();

	// top of memory
	rstack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
	rstack->init();
	cs_stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
	cs_stack->init();
	stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
	stack->init();

	// use the remaing as dictionary space
	dict = reinterpret_cast<Dict*>(mem.alloc_bot(sizeof(Dict)));
	int start_dict = mem.addr(mem.alloc_bot(0));
	int end_dict = mem.addr(mem.alloc_top(0));
	dict->init(start_dict, end_dict);

	// reinit wordbuf to get predictable results in tests
	wordbuf->init();
}

VM::~VM() {
	input->deinit();
}

