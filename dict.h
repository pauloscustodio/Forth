//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "str.h"
#include <string>
#include <vector>
using namespace std;

struct Header {
	int link;			// address of previous header
	int name_addr;		// address of name
	struct {
		bool smudge : 1;
		bool hidden : 1;
		bool immediate : 1;
	} flags;
	int does;			// address of DOES> code
	int code;			// primitive code

	CString* name() const;
	int xt() const;
	int body() const;
	static Header* header(int xt);
};


class Dict {
public:
	void init(int lo_mem, int hi_mem);
	void clear();

	int latest() const { return latest_; }
	int here() const { return here_; }
	int names() const { return names_; }

	void set_latest(int latest) { latest_ = latest; }
	void set_here(int here) { here_ = here; }
	void set_names(int names) { names_ = names; }

	void allot(int size);
	int unused() const;

	int parse_create(int code, int flags); // return xt of word

	int create(const string& name, int flags, int code); // return xt of word
	int create(const char* name, size_t size, int flags, int code); // return xt of word
	int create(const char* name, int size, int flags, int code); // return xt of word
	int create(const CString* name, int flags, int code); // return xt of word

	int alloc_cstring(const string& str);
	int alloc_cstring(const char* str, size_t size);
	int alloc_cstring(const char* str, int size);
	int alloc_cstring(const CString* str);

	int alloc_string(const string& str);
	int alloc_string(const char* str, size_t size);
	int alloc_string(const char* str, int size);
	int alloc_string(const LongString* str);

	void ccomma(int value);
	void comma(int value);
	void dcomma(dint value);
	void align();

	Header* parse_find_word();
	Header* parse_find_existing_word();

	Header* find_word(const string& name) const;
	Header* find_word(const char* name, size_t size) const;
	Header* find_word(const char* name, int size) const;
	Header* find_word(const CString* name) const;

	vector<string> get_words() const;

private:
    int lo_mem_, hi_mem_;// memory limits
	int latest_;			// point to last defined word header
	int here_;			// point to next free position at bottom of memory
	int names_;			// point to last name created at top of memory

	void check_free_space(int size = 0) const;
	int create_cont(int name_addr, int flags, int code);
};


void f_find(int addr);	// search dictionary, word max size 255

int f_tick();
void f_bracket_tick();

void f_postpone();
void f_bracket_compile();
void f_immediate();
void f_hidden();

void f_create();
void f_buffer_colon();
void f_variable();
void f_2variable();
void f_value();
void f_to();
void f_constant();
void f_2constant();
void f_does();
void f_xdoes_define();
void f_xdoes_run(int body);

void f_marker();
void f_xmarker(int body);

void f_words();

void f_defer();
void f_xdefer(int body);
void f_defer_fetch();
void f_defer_fetch(int xt);
void f_defer_store();
void f_action_of();
void f_is();
