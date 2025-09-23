//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>

void clear_locals();
void f_paren_local();
void f_paren_local(const char* name, uint size);
void f_paren_local(const std::string& name);
void f_locals_bar();
void f_locals_bracket();
bool find_local(const std::string& name, uint& index);
bool find_local(const char* name, uint size, uint& index);
