//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>

extern int g_argc;
extern char** g_argv;
extern bool g_interactive;

void f_environment_q();
void f_environment_q(const std::string& query);
void f_environment_q(const char* query, uint size);
