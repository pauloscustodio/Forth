//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

// outer interpreter
void interpret_word(const string& word);
void interpret_word(const char* word, uint size);
void f_interpret();

// evaluate text
void f_evaluate();
void f_evaluate(const string& text);
void f_evaluate(const char* text, uint size);

// main loop
void f_quit();

