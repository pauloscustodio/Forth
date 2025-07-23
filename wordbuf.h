//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

struct CountedString {
	uchar size;
	char str[1];

	string to_string() const;
};

class Wordbuf {
public:
	void init() { m_ptr = 0; }

	CountedString* append(const string& str);
	CountedString* append(const char* str, size_t size);
	CountedString* append(const char* str, int size);

private:
	char m_buffer[BUFFER_SZ];
	int m_ptr;
};
