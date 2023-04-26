//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"

static const char* lookup_error_msg(int error_code) {
    switch (error_code) {
#define X(id, code, message)    case id: return message;
#include "errors.def"
    default:
        return NULL;
    }
}

void error(int error_code) {
    error_arg(error_code, NULL);
}

void error_arg(int error_code, const char* arg) {
    if (error_code == 0)
        return;

    const char* error_msg = lookup_error_msg(error_code);
    fprintf(stderr, "\nError: ");
    if (error_msg)
        fprintf(stderr, "%s", error_msg);
    else
        fprintf(stderr, "%d", error_code);
    if (arg)
        fprintf(stderr, ": %s", arg);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}
