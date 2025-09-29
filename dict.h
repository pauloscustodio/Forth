//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "str.h"
#include <string>
#include <vector>

struct Header {
    uint link;			// address of previous header
    uint name_addr;		// address of name
    struct {
        bool smudge : 1;
        bool hidden : 1;
        bool immediate : 1;
    } flags;
    uint size;			// size of body, filled by next header
    uint creator_xt;		// xt of word that created this word
    uint does;			// address of DOES> code
    uint code;			// primitive code

    CString* name() const;
    uint xt() const;
    uint body() const;
    static Header* header(uint xt);
    uint get_size() const;
};


class Dict {
public:
    void init();
    void clear();

    void allot(int size);
    int unused() const;

    uint parse_create(uint code, int flags); // return xt of word

    uint create(const std::string& name, int flags, uint code); // return xt of word
    uint create(const char* name, uint size, int flags,
                uint code); // return xt of word
    uint create(const CString* name, int flags, uint code); // return xt of word

    uint alloc_cstring(const std::string& str);
    uint alloc_cstring(const char* str, uint size);
    uint alloc_cstring(const CString* str);

    uint alloc_string(const std::string& str);
    uint alloc_string(const char* str, uint size);
    uint alloc_string(const LongString* str);

    void ccomma(int value);
    void comma(int value);
    void dcomma(dint value);
    void fcomma(double value);
    void align();

    Header* parse_find_word();
    Header* parse_find_existing_word();

    Header* find_word(const std::string& name) const;
    Header* find_word(const char* name, uint size) const;
    Header* find_word(const CString* name) const;

    std::vector<std::string> get_words(uint wid = 0) const;
    std::vector<uint> get_word_nts(uint wid = 0) const;

private:
    void check_free_space(int size = 0) const;
    uint create_cont(uint name_addr, int flags, uint code);
};


void f_find(uint addr);	// search dictionary, word max size 255

int f_tick();
void f_bracket_tick();

void f_postpone();
void f_bracket_compile();
void f_compile_comma();
void f_immediate();
void f_hidden();

void f_create();
void f_buffer_colon();
void f_variable();
void f_2variable();
void f_fvariable();
void f_value();
void f_two_value();
void f_fvalue();
void f_to();
void f_constant();
void f_2constant();
void f_fconstant();
void f_does();
void f_xdoes_define();
void f_xdoes_run(uint body);

void f_marker();
void f_xmarker(uint body);

void f_words();

void f_defer();
void f_xdefer(uint body);
void f_defer_fetch();
void f_defer_fetch(uint xt);
void f_defer_store();
void f_action_of();
void f_is();
