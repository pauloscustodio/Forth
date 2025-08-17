//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

enum class Error {
#define X(code, id, message) id = code,
#include "errors.def"
};

[[noreturn]] void error(Error err, const string& arg = "");
[[noreturn]] void error(int err, const string& arg = "");
