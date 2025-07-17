//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
using namespace std;

class Pad {
public: 
	void init();

	char* pad() { return m_pad; }

private:
	char m_pad[PAD_SZ];
};


class Tib {
public:
	void init();

	void refill(const char* str, int size);
	
	char* tib() { return m_tib; }
	
	int size() const { return m_size; }
	int ptr() const { return m_ptr; }

	int* size_ptr() { return &m_size; }
	int* ptr_ptr() { return &m_ptr; }

	bool empty() const { return m_ptr >= m_size; }
	void skip_blanks();
	void skip_blank();
	int skip_to_delimiter(char delimiter = BL); // return end pointer, char before delimiter

private:
	char m_tib[BUFFER_SZ];
	int m_size;
	int m_ptr;
};


class Wordbuf {
public:
	void init();

	char* append(const char* str, int size);

private:
	char m_buffer[BUFFER_SZ];
	int m_ptr;
};

