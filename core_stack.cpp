//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "core_stack.h"
#include "errors.h"
#include <cstring>
#include <iostream>
using namespace std;

void Stack::init() {
	memset(m_stack, 0, sizeof(m_stack));
	m_ptr = STACK_SZ;
}

void Stack::push(int value) {
	if (m_ptr <= 0)
		error(Error::StackOverflow);
	else
		m_stack[--m_ptr] = value;
}

int Stack::pop() {
	if (m_ptr >= STACK_SZ) {
		error(Error::StackUnderflow);
		return 0;
	}
	else {
		return m_stack[m_ptr++];
	}
}

int Stack::peek(int depth) const {
	if (depth < 0) {
		error(Error::InvalidMemoryAddress);
		return 0;
	}
	else {
		int idx = m_ptr + depth;
		if (idx >= STACK_SZ) {
			error(Error::StackUnderflow);
			return 0;
		}
		else {
			return m_stack[idx];
		}
	}
}

void Stack::print() const {
	cout << "( ";
	for (int i = STACK_SZ - 1; i >= m_ptr; i--)
		cout << m_stack[i] << " ";		// TODO: use Forth output to respect BASE
	cout << ") ";
}
