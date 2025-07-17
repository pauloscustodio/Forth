//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "memory.h"
#include "parse.h"
#include "stack.h"
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
};

extern VM vm;


