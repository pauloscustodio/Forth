//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include <iostream>
#include <string>
using namespace std;

[[noreturn]] static void output_error(const string& message, const string& arg) {
    cerr << endl << "Error: " << message;
    if (!arg.empty())
        cerr << ": " << arg;
    cerr << endl;
    exit(EXIT_FAILURE);
}

[[noreturn]] void error(Error err, const string& arg) {
    if (err == Error::None)
        exit(EXIT_SUCCESS);
    else {
        switch (err) {
#define X(code, id, message) case Error::id: output_error(message, arg); break;
#include "errors.def"
        default:
            output_error("Unknown error", std::to_string(static_cast<int>(err)));
            break;
        }
    }
}

void error(int err, const string& arg) {
	error(static_cast<Error>(err), arg);
}
