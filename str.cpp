//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "parser.h"
#include "str.h"
#include "vm.h"
#include <cstring>
using namespace std;

string CString::to_string() const {
    return string(str_, str_ + size_);
}

int CString::alloc_size(int num_chars) {
    if (num_chars > MAX_CSTRING_SZ)
        error(Error::ParsedStringOverflow, std::to_string(num_chars));

    return aligned(1 + num_chars + 1); // count + chars + BL after string
}

void CString::set_cstring(const char* str, int size) {
    if (size > MAX_CSTRING_SZ)
        error(Error::ParsedStringOverflow, string(str, str + size));

    size_ = static_cast<uchar>(size);
    memcpy(str_, str, size);
    str_[size] = BL; // BL after the string
}

void CString::set_cstring(const char* str, size_t size) {
    set_cstring(str, static_cast<int>(size));
}

void CString::set_cstring(const string& str) {
    set_cstring(str.c_str(), str.size());
}

string LongString::to_string() const {
	return string(str_, str_ + size_);
}

int LongString::alloc_size(int num_chars) {
	if (num_chars > BUFFER_SZ)
		error(Error::InputBufferOverflow, std::to_string(num_chars));

	return aligned(sizeof(size_) + num_chars + 1); // count + chars + BL after string
}

void LongString::set_string(const char* str, int size) {
	if (size > BUFFER_SZ)
		error(Error::InputBufferOverflow, string(str, str + size));

	size_ = size;
	memcpy(str_, str, size);
	str_[size] = BL; // BL after the string
}

void LongString::set_string(const char* str, size_t size) {
	set_string(str, static_cast<int>(size));
}

void LongString::set_string(const string& str) {
	set_string(str.c_str(), str.size());
}

void Wordbuf::init() {
	memset(data_, BL, sizeof(data_));
	ptr_ = 0;
}

CString* Wordbuf::append(const string& str) {
	return append(str.c_str(), str.size());
}

CString* Wordbuf::append(const char* str, size_t size) {
	return append(str, static_cast<int>(size));
}

CString* Wordbuf::append(const char* str, int size) {
    if (size > MAX_CSTRING_SZ)
        error(Error::ParsedStringOverflow, string(str, str + size));
    
    int alloc_size = CString::alloc_size(size);
	if (ptr_ + alloc_size > static_cast<int>(sizeof(data_)))
		ptr_ = 0;
	CString* cstring = reinterpret_cast<CString*>(data_ + ptr_);
	ptr_ += alloc_size;

	cstring->set_cstring(str, size);

	return cstring;
}


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
	int len = cfetch(addr++);
	push(addr);
	push(len);
}

void f_dot_quote() {
	int size;
	const char* message = parse_word(size, '"');
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_string(message, size);
		comma(xtXDOT_QUOTE);
		comma(str_addr);
	}
	else {
		print_string(message, size);
	}
}

void f_xdot_quote() {
	int str_addr = fetch(vm.ip); vm.ip += CELL_SZ;
	const LongString* message = reinterpret_cast<const LongString*>(mem_char_ptr(str_addr));
	print_string(message->str(), message->size());
}

void f_s_quote() {
	int size;
	const char* message = parse_word(size, '"');
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_string(message, size);
		comma(xtXS_QUOTE);
		comma(str_addr);
	}
	else {
		push(mem_addr(message));
		push(size);
	}
}

void f_s_backslash_quote() {
	const LongString* message = parse_backslash_string();
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_string(message->str(), message->size());
		comma(xtXS_QUOTE);
		comma(str_addr);
	}
	else {
		push(mem_addr(message->str()));
		push(message->size());
	}
}

void f_xs_quote() {
	int str_addr = fetch(vm.ip); vm.ip += CELL_SZ;
	const LongString* message = reinterpret_cast<const LongString*>(mem_char_ptr(str_addr));
	push(mem_addr(message->str()));
	push(message->size());
}

void f_c_quote() {
	const CString* message = parse_cword('"');
	if (vm.user->STATE == STATE_COMPILE) {
		int str_addr = vm.dict->alloc_cstring(message);
		comma(xtXC_QUOTE);
		comma(str_addr);
	}
	else
		push(mem_addr(message));
}

void f_xc_quote() {
	int str_addr = fetch(vm.ip); vm.ip += CELL_SZ;
	const CString* message = reinterpret_cast<const CString*>(mem_char_ptr(str_addr));
	push(mem_addr(message));
}

void f_dot_paren() {
	int size;
	const char* message = parse_word(size, ')');
	print_string(message, size);
}

