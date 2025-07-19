//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>
using namespace std;

struct Header {
	int prev_addr;		// address of previous header
	int name_addr;		// address of name
	uchar flags;		// F_HIDDEN/F_IMMEDIATE/S_SMUDGE
	func_ptr_t f_does;	// DOES> word
	func_ptr_t f_word;	// executable word

	string name() const;
	int name_size() const;
	int xt() const;
	Header* header(int xt);
};

class Dict {
public:
	void init(int lo_mem, int hi_mem);
	int create(const char* name, int flags, func_ptr_t f_word); // return xt of word
	int create(const char* name, int size, int flags, func_ptr_t f_word); // return xt of word

	int latest() const { return m_latest; }

	void ccomma(int value);
	void comma(int value);
	void dcomma(dint value);
	void align();

private:
	int m_lo_mem;
	int m_hi_mem;
	int m_latest;		// point to last defined word header
	int m_here;			// point to next free position at bottom of memory
	int m_names;		// point to last name created at top of memory

	void check_free_space(int size = 0) const;
};
