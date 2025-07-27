//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "environ.h"
using namespace std;

int g_argc = 0;
char** g_argv = nullptr;
bool g_interactive = false;
