//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

using namespace std;

bool case_insensitive_equal(const string& a, const string& b);
bool case_insensitive_equal(const char* a_str, int a_size, const char* b_str, int b_size);
bool case_insensitive_equal(const char* a_str, size_t a_size, const char* b_str, size_t b_size);

void f_count();
void f_dot_quote();
void f_xdot_quote();
void f_s_quote();
void f_xs_quote();
void f_c_quote();
void f_xc_quote();
void f_dot_paren();
