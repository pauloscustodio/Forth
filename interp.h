//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>

// outer interpreter
void interpret_word(const std::string& word);
void interpret_word(const char* word, uint size);
void f_interpret();

// evaluate text
void f_evaluate();
void f_evaluate(const std::string& text);
void f_evaluate(const char* text, uint size);

// main loop
void f_quit();

