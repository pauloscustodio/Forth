//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "vm.h"
#include <iostream>
#include <vector>
using namespace std;


// alignment and double cells
int align(int x) {
	return (x + CELL_SZ - 1) & ~(CELL_SZ - 1);
}

int dcell_lo(dint x) {
	return x & 0xffffffffLL;
}

int dcell_hi(dint x) {
	return (x >> 32) & 0xffffffffLL;
}

int dcell(int hi, int lo) {
	return (static_cast<udint>(dcell_lo(hi)) << 32) |
		static_cast<udint>(dcell_lo(lo));
}

int peek(int depth) { 
	return vm.stack->peek(depth);
}

int pop() { 
	return vm.stack->pop();
}

void push(int value) { 
	vm.stack->push(value); 
}

void rpush(int value) { 
	vm.rstack->push(value); 
}

int rpop() { 
	return vm.rstack->pop(); 
}

int rpeek(int depth) { 
	return vm.rstack->peek(depth);
}

char* c_WORD(char delimiter) {
	if (delimiter == BL)
		vm.tib->skip_blanks();	// skip blanks before word
	else
		vm.tib->skip_blank();	// skip space after quote

	int start = vm.tib->ptr();
	int end = vm.tib->skip_to_delimiter(delimiter);

	if (end > start) {
		int size = end - start;
		char* ret = vm.wordbuf->append(vm.tib->tib() + start, size);
		return ret;
	}
	else
		return nullptr;
}

void f_PAD() {
	push(vm.mem.addr(vm.pad->pad()));
}

void f_DOT() {
	int value = pop();
	cout << value << BL;		// TODO: use Forth output to respect BASE
}

void f_THROW() {
	int error_code = pop();
	error(static_cast<Error>(error_code));
}

void f_DROP() {
	pop();
}

void f_DUP() {
	int value = peek(0);
	push(value);
}

void f_PICK() {
	int depth = pop();
	if (depth < 0) {
		error(Error::InvalidMemoryAddress);
		return;
	}
	int value = peek(depth);
	push(value);
}

void f_PLUS() {
	int b = pop();
	int a = pop();
	push(a + b);
}

void f_WORDS() {
	static vector<string> words = {
		//@@BEGIN: Words
		"WORD",
		".S",
		"S\"",
		"COUNT",
		"TYPE",
		"ENVIRONMENT?",
		"WORDS",
		"+",
		"PICK",
		"DUP",
		"DROP",
		"THROW",
		".",
		"PAD",
		"C@",
		"C!",
		"@",
		"!",
		//@@END
	};

	size_t col = 0;
	for (auto& word : words) {
		if (col + 1 + word.size() >= SCREEN_WIDTH) {
			cout << endl << word;
			col = word.size();
		}
		else if (col == 0) {
			cout << word;
			col += word.size();
		}
		else {
			cout << BL << word;
			col += 1 + word.size();
		}
	}
	cout << endl;
}

void f_ENVIRONMENT_Q() {
	int size = pop();
	int addr = pop();
	const char* str = vm.mem.char_ptr(addr);
	string query{ str, str + size };
	if (query == "CORE") {
		push(F_TRUE);
		push(F_TRUE);
	}
	else {
		push(F_FALSE);
	}
}

void f_TYPE() {
	int size = pop();
	int addr = pop();
	const char* str = vm.mem.char_ptr(addr);
	for (int i = 0; i < size; ++i)
		cout << str[i];
}

void f_COUNT() {
	int addr = pop(); 
	int len = *vm.mem.char_ptr(addr++); 
	push(addr); 
	push(len);
}

void f_S_QUOTE() {
	push('"');
	f_WORD();
}

void f_DOT_S() {
	vm.stack->print();
}

void f_WORD() {
	char delimiter = pop();
	char* word = c_WORD(delimiter);
	if (word) {
		push(vm.mem.addr(word + 1));			// address of word
		push(*reinterpret_cast<uchar*>(word));	// lebgth of word
	}
	else {	// TODO: refill from input
		push(0);
		push(0);
	}
}
