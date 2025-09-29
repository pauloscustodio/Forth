//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

void f_dump();
void f_dump(const char* mem, uint size);
void f_see();
void f_n_to_r();
void f_n_r_from();
void f_forget();
void f_to_name();
void f_name_to_compile();
void f_name_to_string();
void f_name_to_interpret();
void f_synonym();
void f_xsynonym(uint body);
void f_traverse_wordlist();
