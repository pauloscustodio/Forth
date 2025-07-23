//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

class CountedString {
public:
	string to_string() const;
	int size() const { return m_size; }
	const char* str() const { return m_str; }
	void set_string(const char* str, int size);		// space must be allocated by user

private:
	uchar m_size;			// Forth COUNTed string starts here; max 255
	char m_str[1];			// extended to length of string plus one blank
};

//-----------------------------------------------------------------------------

class ForthString {
public:
	string to_string() const;
	int size() const { return m_size; }
	const char* str() const { return m_str; }
	const CountedString* counted_string() const;
	void set_string(const char* str, int size);		// space must be allocated by user

	static int alloc_size(int str_size);	// return aligned size of structure + characters + BL

private:
	int m_size;				// Full size
	uchar m_short_size;		// Forth COUNTed string starts here; max 255
	char m_str[1];			// extended to length of string plus one blank

	void set_size(size_t size);
	void set_size(int size);
};

//-----------------------------------------------------------------------------

class Wordbuf {
public:
	void init() { m_ptr = 0; }

	ForthString* append(const string& str);
	ForthString* append(const char* str, size_t size);
	ForthString* append(const char* str, int size);

private:
	char m_buffer[WORDBUF_SZ];
	int m_ptr;
};
