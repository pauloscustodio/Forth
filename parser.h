//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "input.h"
#include "forth.h"
using namespace std;

CountedString* parse_word(char delimiter = BL);
bool parse_number(const char* text, size_t size, bool& is_double, dint& value);
bool parse_number(const char* text, int size, bool& is_double, dint& value);

CountedString* cWORD(char delimiter = BL);
