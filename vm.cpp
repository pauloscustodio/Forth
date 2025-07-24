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

int VM::ptr_to_addr(const char* ptr) {
	return static_cast<int>(ptr - reinterpret_cast<char*>(this));
}

void VM::init() {
	memset(&vm, 0, sizeof(vm));
	init_stacks();
}

void VM::init_stacks() {
	vm.sp = STACK_SZ;
	vm.rp = STACK_SZ;
}

void VM::push(int value) {
	if (vm.sp <= 0)
		error(Error::StackOverflow);
	else
		vm.stack[--vm.sp] = value;
}

int VM::pop() {
	if (vm.sp >= STACK_SZ) {
		error(Error::StackUnderflow);
		return 0;
	}
	else {
		return vm.stack[vm.sp++];
	}
}

int VM::peek(int depth) {
	if (depth < 0) {
		error(Error::InvalidMemoryAddress);
		return 0;
	}
	else {
		int idx = vm.sp + depth;
		if (idx >= STACK_SZ) {
			error(Error::StackUnderflow);
			return 0;
		}
		else {
			return vm.stack[idx];
		}
	}
}

void VM::dot_stack() {
	cout << "( ";
	for (int i = STACK_SZ - 1; i >= vm.sp; i--)
		cout << vm.stack[i] << " ";		// TODO: use Forth output to respect BASE
	cout << ") ";
}

void VM::words() {
	static vector<string> words = {
		//@@BEGIN: Words
		".",
		"THROW",
		"DROP",
		"DUP",
		"PICK",
		"+",
		".S",
		"WORDS",
		"S\"",
		"TYPE",
		"ENVIRONMENT?",
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

void VM::refill(const string& text) {
	if (text.size() + 1 > BUFFER_SZ) {
		error(Error::InputBufferOverflow);
		tib_ptr = 0;
		tib_size = 0;
	}
	else {
		memcpy(tib, text.c_str(), text.size() + 1);
		tib_ptr = 0;
		tib_size = static_cast<int>(text.size());
	}
}

bool VM::parse_word(char delimiter) {
	if (delimiter == ' ') {
		while (tib_ptr < tib_size && isspace(static_cast<unsigned char>(tib[tib_ptr])))
			++tib_ptr;
	}
	else {
		if (tib_ptr < tib_size)
			++tib_ptr;	// skip space after quote
	}
	
	int start = tib_ptr;
	int end = start;
	if (delimiter == ' ') {
		while (tib_ptr < tib_size && !isspace(static_cast<unsigned char>(tib[tib_ptr])))
			++tib_ptr;
		end = tib_ptr;
	}
	else {
		while (tib_ptr < tib_size&& tib[tib_ptr] != delimiter)
			++tib_ptr;
		end = tib_ptr;
		if (tib[tib_ptr] == delimiter)
			++tib_ptr;	// skip delimiter
	}

	if (end > start) {
		int len = end - start;
		const char* cstr = append_wordbuf(tib + start, len);
		push(ptr_to_addr(cstr + 1));
		push(len);
		return true;
	}
	else
		return false;
}

void VM::f_s_quote() {
	if (!parse_word('"')) {
		push(0);
		push(0);
	}
}

void VM::f_type() {
	int len = pop();
	int addr = pop();
	const char* str = reinterpret_cast<char*>(this) + addr;
	for (int i = 0; i < len; ++i)
		cout << str[i];
}

const char* VM::append_wordbuf(const char* str, int len) {
	if (len > 0xFF) {
		error(Error::ParsedStringOverflow);
		return append_wordbuf("", 0);
	}
	else {
		if (wordbuf_ptr + 1 + len + 1 >= BUFFER_SZ)
			wordbuf_ptr = 0;
		const char* ret = wordbuf + wordbuf_ptr;
		wordbuf[wordbuf_ptr++] = len & 0xFF; // size
		memcpy(wordbuf + wordbuf_ptr, str, len);
		wordbuf_ptr += len;
		wordbuf[wordbuf_ptr++] = 0; // null terminator
		return ret;
	}
}

void VM::f_environment_q() {
	int len = pop();
	int addr = pop();
	const char* str = reinterpret_cast<char*>(this) + addr;
	string query{ str, str + len };
	if (query == "CORE") { push(F_TRUE); push(F_TRUE); }
	else { push(F_FALSE); }
}
