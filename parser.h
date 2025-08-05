//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "str.h"
using namespace std;

const char* parse_word(int& size, char delimiter = BL);
CString* parse_cword(char delimiter = BL);

bool parse_number(const string& text, bool& is_double, dint& value);
bool parse_number(const char* text, size_t size, bool& is_double, dint& value);
bool parse_number(const char* text, int size, bool& is_double, dint& value);

int f_word(int delim);
void f_number_q();
void f_number();
void f_to_number();
void f_convert();

void f_open_paren();
void f_backslash();

int f_char(char delim);
void f_bracket_char(char delim);
