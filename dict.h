//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

// flags for name length
#define F_COUNTED_STR_MASK  0xff
#define F_LENMASK           0x3f
#define F_HIDDEN            0x40
#define F_IMMEDIATE         0x80

void init_dict(void);

void comma(int value);
void dcomma(dint value);
void ccomma(int value);
void counted_str_comma(const char* text, int len);
void dict_name_comma(const char* text, int len, int flags);
void long_str_comma(const char* text, int len);
void align(void);

// dictionary
int  create(const char* name, int len, int flags, int id);
void parse_create(int id);
void f_create(void);

void f_colon(void);
void f_colon_noname(void);
void f_semicolon(void);

void f_constant(void);
void f_2constant(void);

void f_variable(void);
void f_2variable(void);

void f_value(void);
void f_to(void);

void f_does(void);
void f_xdoes_define(void);
void f_xdoes_run(int body);

int  link_to_name(int link);
int  link_to_does(int link);
int  link_to_xt(int link);
int  xt_to_link(int xt);
int  xt_to_body(int xt);
int  body_to_does(int xt);
void toggle_hidden(void);
void toggle_immediate(void);

int  c_find_link(const char* word, int len);
void f_find(int cstr);
int  f_tick(void);
void f_bracket_tick(void);
void f_postpone(void);
void f_bracket_compile(void);

void f_marker(void);
void f_xmarker(int body);

void print_words(void);
