//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

void skip_blanks(void);

const char* c_parse(int delim, int* len);
const char* c_parse_word(int* len);

void f_parse(int delim);
void f_parse_word(void);
int f_word(int delim);
int f_char(int delim);
void f_bracket_char(int delim);
void f_open_paren(void);
void f_backslash(void);

int c_parse_number(const char* text, int len, dint* value);
void f_number_q(void);
void f_number(void);
void f_to_number(void);
void f_convert(void);
