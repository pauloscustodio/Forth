//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "wordbuf.h"
#include <string>
#include <vector>
using namespace std;

struct Header {
	int link;			// address of previous header
	int name_addr;		// address of name
	struct {
		bool hidden : 1;
		bool immediate : 1;
		bool smudge : 1;
	} flags;
	int does;			// address of DOES> code
    int code;			// either primitive code or address of xt of word

	ForthString* name() const;
	int name_size() const;
	int xt() const;
	int body() const;
	static Header* header(int xt);
};

class Dict {
public:
	void init(int lo_mem, int hi_mem);
	int create(const char* name, int flags, int code); // return xt of word
	int create(const char* name, size_t size, int flags, int code); // return xt of word
	int create(const char* name, int size, int flags, int code); // return xt of word
	int create(const ForthString* name, int flags, int code); // return xt of word

	int parse_create(int code, int flags); // return xt of word

	void allot(int size);
	int unused() const;

	int alloc_name(const char* name);
	int alloc_name(const char* name, size_t size);
	int alloc_name(const char* name, int size);
	int alloc_name(const ForthString* name);

	vector<string> get_words() const;

	int latest() const { return m_latest; }
	void set_latest(int latest) { m_latest = latest; }
    int here() const { return m_here; }
	void set_here(int here) { m_here = here; }
    int names() const { return m_names; }
	void set_names(int names) { m_names = names; }

	void ccomma(int value);
	void comma(int value);
	void dcomma(dint value);
	void align();

	Header* find_word(const string& name) const;
    Header* find_word(const char* name, size_t size) const;
    Header* find_word(const char* name, int size) const;
    Header* find_word(const ForthString* name) const;
	Header* parse_find_word();
	Header* parse_find_existing_word();

private:
	int m_latest;		// point to last defined word header
	int m_here;			// point to next free position at bottom of memory
	int m_names;		// point to last name created at top of memory

	void check_free_space(int size = 0) const;
	int create_cont(int name_addr, int flags, int code);
};

void f_find(int addr);	// search dictionary, word max size 255
void f_marker();
void f_xmarker(int body);
void f_words();
void f_create();
void f_colon();
void f_colon_noname();
void f_semicolon();
void f_variable();
void f_2variable();
void f_value();
void f_to();
void f_constant();
void f_2constant();
void f_does();
void f_xdoes_define();
void f_xdoes_run(int body);
int f_tick();
void f_bracket_tick();
void f_postpone();
void f_bracket_compile();
void f_immediate();
void f_hidden();
