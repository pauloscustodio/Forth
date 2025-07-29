//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "math.h"
#include "output.h"
#include "stack.h"
#include "vm.h"
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

void Stack::dpush(dint value) {
	push(dcell_lo(value));
	push(dcell_hi(value));
}

dint Stack::dpop() {
	int hi = pop();
	int lo = pop();
	return dcell(hi, lo);
}

dint Stack::dpeek(int depth) const {
	int hi = peek(2 * depth);
	int lo = peek(2 * depth + 1);
	return dcell(hi, lo);
}

// e.g. roll(1)
// | 1 | 2 |    --> | 2 | 1 |
// ^sp ^bot
void Stack::roll(int depth) {
	if (depth < 0)
		error(Error::InvalidMemoryAddress);
	else if (depth == 0)
		; // ignore
	else {
		int bot = m_ptr + depth;
		if (bot >= STACK_SZ)
			error(Error::StackUnderflow);
		else {
			int bot_value = m_stack[bot];
			memmove(&m_stack[m_ptr + 1], &m_stack[m_ptr], depth * CELL_SZ);
			m_stack[m_ptr] = bot_value;
		}
	}
}

void Stack::print(const string& prefix) const {
	cout << "(";
	if (!prefix.empty())
		cout << prefix << ":";
	cout << " ";
	for (int i = STACK_SZ - 1; i >= m_ptr; i--)
		print_number(m_stack[i]);
	cout << ") ";
}

void Stack::set_ptr(int ptr) {
	if (ptr < 0 || ptr > STACK_SZ)
		error(Error::InvalidMemoryAddress);
	m_ptr = ptr;
}

void Stack::clear() {
	m_ptr = STACK_SZ;
}

