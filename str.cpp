//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "parser.h"
#include "str.h"
#include "vm.h"
#include "wordbuf.h"
using namespace std;

static char to_lower(char c) {
	return tolower(static_cast<unsigned char>(c));
}

bool case_insensitive_equal(const string& a, const string& b) {
	return case_insensitive_equal(a.c_str(), a.size(), b.c_str(), b.size());
}

bool case_insensitive_equal(const char* a_str, int a_size, const char* b_str, int b_size) {
	if (a_size != b_size)
		return false;
	for (int i = 0; i < a_size; ++i) {
		if (to_lower(a_str[i]) != to_lower(b_str[i]))
			return false;
	}
	return true;
}

bool case_insensitive_equal(const char* a_str, size_t a_size, const char* b_str, size_t b_size) {
	return case_insensitive_equal(a_str, static_cast<int>(a_size), b_str, static_cast<int>(b_size));
}

void f_count() {
	int addr = pop();
	int size = cfetch(addr++);
	push(addr);
	push(size);
}

void f_s_quote() {
	const ForthString* word = parse_word('"');
	push(mem_addr(word->str()));	// address of word
	push(word->size());				// length of word
}
