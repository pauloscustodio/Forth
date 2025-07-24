//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "dict.h"
#include "forth.h"
#include "input.h"
#include "memory.h"
#include "output.h"
#include "stack.h"
using namespace std;

class Pad {
public:
	void init() {}

	char* pad() { return m_pad; }

private:
	char m_pad[PAD_SZ];
};


struct VM {
	VM();
	virtual ~VM();

	// memory
	Mem mem;

	// input
	Wordbuf* wordbuf;
	Pad* pad;
	NumberOutput* number_output;
	Input* input;

	// user variables
	User* user;

	// stacks
	Stack* rstack;			// return stack
	Stack* stack;			// data stack

	// dictionary
	Dict* dict;
};

extern VM vm;
