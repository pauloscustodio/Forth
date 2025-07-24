//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "dict.h"
#include "wordbuf.h"
#include <cstring>
#include <string>
using namespace std;

string CountedString::to_string() const {
	return string(m_str, m_str + m_size);
}

void CountedString::set_string(const char* str, int size) {
	if (size > MAX_COUNTED_STRING_SZ)
		error(Error::ParsedStringOverflow);
	m_size = size > MAX_COUNTED_STRING_SZ ? MAX_COUNTED_STRING_SZ : size;
	memcpy(m_str, str, m_size);
	m_str[m_size] = BL;
}

//-----------------------------------------------------------------------------

string ForthString::to_string() const {
	return string(m_str, m_str + m_size);
}

const CountedString* ForthString::counted_string() const { 
	if (m_size > MAX_COUNTED_STRING_SZ)
		error(Error::ParsedStringOverflow);
	return reinterpret_cast<const CountedString*>(&m_short_size); 
}

void ForthString::set_size(size_t size) {
	set_size(static_cast<int>(size));
}

void ForthString::set_size(int size) {
	if (size > MAX_STRING_SZ)
		error(Error::StringTooLong);
	m_size = size;
	m_short_size = size > MAX_COUNTED_STRING_SZ ? MAX_COUNTED_STRING_SZ : size;
}

void ForthString::set_string(const char* str, int size) {
	set_size(size);
	memcpy(m_str, str, size);
	m_str[size] = BL;
}

int ForthString::alloc_size(int str_size) {
	return aligned(offsetof(ForthString, m_str) + str_size + 1);
}

//-----------------------------------------------------------------------------

void Wordbuf::init() { 
    memset(m_buffer, BL, sizeof(m_buffer));
	m_ptr = 0; 
}

ForthString* Wordbuf::append(const string& str) {
	return append(str.c_str(), str.size());
}

ForthString* Wordbuf::append(const char* str, size_t size) {
	return append(str, static_cast<int>(size));
}

ForthString* Wordbuf::append(const char* str, int size) {
	if (size > MAX_STRING_SZ)
		error(Error::StringTooLong);
	int alloc_size = ForthString::alloc_size(size);
	if (m_ptr + alloc_size > static_cast<int>(sizeof(m_buffer)))
		m_ptr = 0;
	ForthString* fstring = reinterpret_cast<ForthString*>(m_buffer + m_ptr);
	
	fstring->set_string(str, size);
	m_ptr += alloc_size;

	return fstring;
}
