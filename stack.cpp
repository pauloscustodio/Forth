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

void Stack::print() const {
	cout << "( ";
	for (int i = STACK_SZ - 1; i >= m_ptr; i--)
		cDOT(m_stack[i]);
	cout << ") ";
}

void push(int value) {
	vm.stack->push(value);
}

int peek(int depth) {
	return vm.stack->peek(depth);
}

int pop() {
	return vm.stack->pop();
}

void dpush(dint value) {
	vm.stack->dpush(value);
}

dint dpeek(int depth) {
	return vm.stack->dpeek(depth);
}

dint dpop() {
	return vm.stack->dpop();
}

void rpush(int value) {
	vm.rstack->push(value);
}

int rpeek(int depth) {
	return vm.rstack->peek(depth);
}

int rpop() {
	return vm.rstack->pop();
}

void fDROP() {
	pop();
}

void fDUP() {
	int value = peek(0);
	push(value);
}

void fPICK() {
	int depth = pop();
	if (depth < 0) {
		error(Error::InvalidMemoryAddress);
		return;
	}
	int value = peek(depth);
	push(value);
}

void fDOT_S() {
	vm.stack->print();
}

