//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>
using namespace std;

// interpreter pointer
extern int ip;

// outer interpreter
void f_interpret_word(const string& word);
void f_interpret_word(const char* word, size_t size);
void f_interpret_word(const char* word, int size);
void f_interpret();

// evaluate text
void f_evaluate();
void f_evaluate(const string& text);
void f_evaluate(const char* text, size_t size);
void f_evaluate(const char* text, int size);
