//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2022
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

// errors
static const char* error_lu[] = {
#define X(code, message)    message,
#include "errors.def"
};

void init_errors(void) {
}

void error(ErrorCode code) {
    error_arg(code, NULL);
}

void error_arg(ErrorCode code, const char* arg) {
    fprintf(stderr, "\nError: %s", error_lu[code]);
    if (arg)
        fprintf(stderr, ": %s", arg);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}
