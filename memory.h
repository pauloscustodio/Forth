//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

class Mem {
public:
	Mem();

	// pointer - address conversion
	int addr(const char* ptr) const;
	int addr(const int* ptr) const;
	char* char_ptr(int addr, int size = 0);
	int* int_ptr(int addr, int size = 0);

	// access memory
	int fetch(int addr);
	void store(int addr, int value);
	dint dfetch(int addr);
	void dstore(int addr, dint value);
	int cfetch(int addr);
	void cstore(int addr, int value);

	// allocate memory for data structures
	char* alloc_bot(int size);
	char* alloc_top(int size);

private:
	char m_mem[MEM_SZ];
	int m_bot;
	int m_top;

	int check_addr(int addr, int size = 0) const;
};
