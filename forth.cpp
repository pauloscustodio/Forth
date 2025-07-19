//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "parse.h"
#include "vm.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

//@@BEGIN: WordsXtDefinition
int xtBASE = 0; // BASE
int xtSTATE = 0; // STATE
int xtSTORE = 0; // !
int xtFETCH = 0; // @
int xtC_STORE = 0; // C!
int xtC_FETCH = 0; // C@
int xtPAD = 0; // PAD
int xtDOT = 0; // .
int xtTHROW = 0; // THROW
int xtDROP = 0; // DROP
int xtDUP = 0; // DUP
int xtPICK = 0; // PICK
int xtPLUS = 0; // +
int xtENVIRONMENT_Q = 0; // ENVIRONMENT?
int xtTYPE = 0; // TYPE
int xtCOUNT = 0; // COUNT
int xtS_QUOTE = 0; // S"
int xtDOT_S = 0; // .S
int xtWORD = 0; // WORD
int xtWORDS = 0; // WORDS
int xtFIND = 0; // FIND
int xtCOMMA = 0; // ,
int xtC_COMMA = 0; // C,
int xtALIGN = 0; // ALIGN
//@@END

//@@BEGIN: VarsImplementation
void fBASE() { push(vm.mem.addr(&vm.vBASE)); }
void fSTATE() { push(vm.mem.addr(&vm.vSTATE)); }
//@@END

// alignment and double cells
int aligned(int x) {
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

void fVOID() {	// do nothing
}

CountedString* cWORD(char delimiter) {
	if (delimiter == BL)
		vm.tib->skip_blanks();	// skip blanks before word
	else
		vm.tib->skip_blank();	// skip space after quote

	int start = vm.tib->ptr();
	int end = vm.tib->skip_to_delimiter(delimiter);

	if (end > start) {
		int size = end - start;
		CountedString* ret = vm.wordbuf->append(vm.tib->tib() + start, size);
		return ret;
	}
	else
		return nullptr;
}

Header* cFIND(const char* name, bool& is_immediate) {
	return cFIND(name, static_cast<int>(strlen(name)), is_immediate);
}

Header* cFIND(const char* name, int size, bool& is_immediate) {
	string s_name{ name, name + size };
	is_immediate = false;
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(ptr));
		if (size == header->name_size()) {
			string s_found_name = header->name();
			if (case_insensitive_equal(s_name, s_found_name)) {
				is_immediate = (header->flags & F_IMMEDIATE) ? true : false;
				return header;
			}
		}
		ptr = header->prev_addr;
	}
	
	return nullptr;
}

void fFIND() {
	int addr = pop();
	CountedString* word = reinterpret_cast<CountedString*>(vm.mem.char_ptr(addr));
	bool is_immediate = F_FALSE;
	Header* header = cFIND(word->str, word->size, is_immediate);
	if (header == nullptr) {
		push(addr);
		push(0);
	}
	else {
		int xt = header->xt();
		if (is_immediate) {
			push(xt);
			push(1);
		}
		else {
			push(xt);
			push(-1);
		}
	}
}

vector<string> cWORDS() {
	vector<string> words;
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(ptr));
		string s_found_name = header->name();
		if ((header->flags & F_HIDDEN) == 0) {
			words.push_back(s_found_name);
		}
		ptr = header->prev_addr;
	}
	return words;
}

void fWORDS() {
	vector<string> words = cWORDS();
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

bool case_insensitive_equal(const string& a, const string& b) {
	return a.size() == b.size() &&
		equal(a.begin(), a.end(), b.begin(), [](char c1, char c2) {
		return tolower(static_cast<uchar>(c1)) ==
			tolower(static_cast<uchar>(c2));
			});
}

void fPAD() {
	push(vm.mem.addr(vm.pad->pad()));
}

void fDOT() {
	int value = pop();
	cout << value << BL;		// TODO: use Forth output to respect BASE
}

void fTHROW() {
	int error_code = pop();
	error(static_cast<Error>(error_code));
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

void fPLUS() {
	int b = pop();
	int a = pop();
	push(a + b);
}

void fENVIRONMENT_Q() {
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

void fTYPE() {
	int size = pop();
	int addr = pop();
	const char* str = vm.mem.char_ptr(addr);
	for (int i = 0; i < size; ++i)
		cout << str[i];
}

void fCOUNT() {
	int addr = pop(); 
	int len = *vm.mem.char_ptr(addr++); 
	push(addr); 
	push(len);
}

void fS_QUOTE() {
	push('"');
	fWORD();
}

void fDOT_S() {
	vm.stack->print();
}

void fWORD() {
	char delimiter = pop();
	CountedString* word = cWORD(delimiter);
	if (word) {
		push(vm.mem.addr(word->str));			// address of word
		push(word->size);						// length of word
	}
	else {	// TODO: refill from input
		push(0);
		push(0);
	}
}
