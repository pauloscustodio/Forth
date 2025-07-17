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

VM::VM() {
	// bottom of memory
	pad = reinterpret_cast<Pad*>(mem.alloc_bot(sizeof(Pad)));
	pad->init();
	tib = reinterpret_cast<Tib*>(mem.alloc_bot(sizeof(Tib)));
	tib->init();
	wordbuf = reinterpret_cast<Wordbuf*>(mem.alloc_bot(sizeof(Wordbuf)));
	wordbuf->init();

	// top of memory
	rstack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
	rstack->init();
	stack = reinterpret_cast<Stack*>(mem.alloc_top(sizeof(Stack)));
	stack->init();
}

