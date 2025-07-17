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

void VM::f_s_quote() {
	push('"');
	f_word();
}

void VM::f_type() {
	int size = pop();
	int addr = pop();
	const char* str = reinterpret_cast<char*>(this) + addr;
	for (int i = 0; i < size; ++i)
		cout << str[i];
}

void VM::f_environment_q() {
	int size = pop();
	int addr = pop();
	const char* str = reinterpret_cast<char*>(this) + addr;
	string query{ str, str + size };
	if (query == "CORE") { push(F_TRUE); push(F_TRUE); }
	else { push(F_FALSE); }
}

char* c_word(char delimiter) {
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

void f_word() {
	char delimiter = pop();
	char* word = c_word(delimiter);
	if (word) {
		push(vm.mem.addr(word + 1));			// address of word
		push(*reinterpret_cast<uchar*>(word));	// lebgth of word
	}
	else {	// TODO: refill from input
		push(0);
		push(0);
	}
}

void f_words() {
	static vector<string> words = {
		//@@BEGIN: Words
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
		"C@",
		"C!",
		"@",
		"!",
		"PAD",
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
