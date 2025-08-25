//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "facility.h"

#ifndef _WIN32
#include <cstdio>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

void init_console() {
}

#endif
