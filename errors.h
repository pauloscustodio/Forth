//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

typedef enum ErrorCode {
#define X(code, message)    code,
#include "errors.def"
} ErrorCode;

void init_errors(void);
void error(ErrorCode code);
void error_arg(ErrorCode code, const char* arg);
