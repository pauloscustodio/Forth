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

void f_dot_quote() {
	const ForthString* message = parse_word('"');
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_name(message);
		comma(xtXDOT_QUOTE);
		comma(str_addr);
	}
	else {
		print_string(message->str(), message->size());
	}
}

void f_xdot_quote() {
	int str_addr = fetch(ip); ip += CELL_SZ;
	const ForthString* message = reinterpret_cast<const ForthString*>(mem_char_ptr(str_addr));
	print_string(message->str(), message->size());
}

void f_s_quote() {
	const ForthString* message = parse_word('"');
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_name(message);
		comma(xtXS_QUOTE);
		comma(str_addr);
	}
	else {
		push(mem_addr(message->str()));
		push(message->size());
	}
}

void f_xs_quote() {
	int str_addr = fetch(ip); ip += CELL_SZ;
	const ForthString* message = reinterpret_cast<const ForthString*>(mem_char_ptr(str_addr));
	push(mem_addr(message->str()));
	push(message->size());
}

void f_c_quote() {
	const ForthString* message = parse_word('"');
	const CountedString* cmessage = message->counted_string();
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_name(message);
		comma(xtXC_QUOTE);
		comma(str_addr);
	}
	else
		push(mem_addr(cmessage));
}

void f_xc_quote() {
	int str_addr = fetch(ip); ip += CELL_SZ;
	const ForthString* message = reinterpret_cast<const ForthString*>(mem_char_ptr(str_addr));
	const CountedString* cmessage = message->counted_string();
	push(mem_addr(cmessage));
}

void f_dot_paren() {
	const ForthString* message = parse_word(')');
	print_string(message->str(), message->size());
}
