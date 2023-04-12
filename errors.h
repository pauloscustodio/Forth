//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

enum ErrorCode {
#define X(id, code, message)    code = id,
#include "errors.def"
};

void error(int error_code);
void error_arg(int error_code, const char* arg);
