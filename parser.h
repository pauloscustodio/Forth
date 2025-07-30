//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "input.h"
#include "wordbuf.h"
#include <string>
using namespace std;

const ForthString* parse_word(char delimiter = BL); // empty string if no more words in TIB
void f_parse();
void f_parse_word();
void f_word();
void f_char();
void f_bracket_char();

bool parse_number(const string& text, bool& is_double, dint& value);
bool parse_number(const char* text, size_t size, bool& is_double, dint& value);
bool parse_number(const char* text, int size, bool& is_double, dint& value);

void f_number_q();
void f_number();
void f_to_number();
void f_convert();
