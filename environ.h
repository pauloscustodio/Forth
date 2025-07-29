//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

extern int g_argc;
extern char** g_argv;
extern bool g_interactive;

void f_environment_q(const string& query);
void f_environment_q(const char* query, size_t size);
void f_environment_q(const char* query, int size);
