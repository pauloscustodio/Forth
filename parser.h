//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "input.h"
#include "wordbuf.h"
using namespace std;

const ForthString* f_parse_word(char delimiter = BL); // nullptr if no more words in TIB
bool f_parse_number(const char* text, size_t size, bool& is_double, dint& value);
bool f_parse_number(const char* text, int size, bool& is_double, dint& value);
