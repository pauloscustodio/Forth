//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
using namespace std;

class Stack {
public:
	void init();

	void push(int value);
	int pop();
	int peek(int depth = 0) const;
	void print() const;

private:
	int m_stack[STACK_SZ];
	int m_ptr;
};
