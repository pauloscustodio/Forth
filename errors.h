//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>

enum class Error {
#define X(code, id, message) id = code,
#include "errors.def"
};

void error(Error err, const std::string& arg = "");

void f_catch();
void f_catch(uint xt);
void f_throw();
void f_throw(Error err);
void f_throw(int error_code);

// abort
void f_abort();
void f_abort_quote();
void f_xabort_quote();
