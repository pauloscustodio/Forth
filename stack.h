//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

class Stack {
public:
	void init();

	void push(int value);
	int pop();
	int peek(int depth = 0) const;
	void dpush(dint value);
	dint dpop();
	dint dpeek(int depth = 0) const;
    int depth() const { return STACK_SZ - m_ptr; }
	void roll(int depth);

    int ptr() const { return m_ptr; }
	void set_ptr(int ptr);
	void clear();

	void print(const string& prefix = "") const;

private:
	int m_stack[STACK_SZ];
	int m_ptr;
};

