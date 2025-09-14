//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "str.h"
#include <string>

bool is_space(char c);
bool is_print(char c);

const char* parse_word(uint& size, char delimiter = BL);
CString* parse_cword(char delimiter = BL);
std::string parse_backslash_string();

bool parse_number(const std::string& text, bool& is_double, dint& value);
bool parse_number(const char* text, uint size, bool& is_double, dint& value);

bool parse_float(const std::string& text, double& value);
bool parse_float(const char* text, uint size, double& value);

int f_word(char delimiter);
void f_parse(char delimiter);
void f_parse_name();

int f_char(char delimiter);
void f_bracket_char(char delimiter);

void f_number_q();
void f_number();
void f_to_number();
void f_convert();

void f_open_paren();
void f_backslash();

