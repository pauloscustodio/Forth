//-----------------------------------------------------------------------------'words
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "dict.h"
#include "errors.h"
#include "interp.h"
#include "io.h"
#include "memory.h"
#include "stack.h"
#include <assert.h>
#include <stdlib.h>

void init(void) {
    init_errors();
    init_memory();
    init_stacks();
    init_dict();
    init_io();
}

void f_environment_q(void) {
    int len = pop();
    int addr = pop();
    const char* str = fstr_to_cstr(mem + addr, len);

#define QUERY_ENV(env, action)  \
        else if (len==sizeof(env)-1 && 0==strcasecmp(str, env)) { action; }
    if (0) {}
    QUERY_ENV("/COUNTED-STRING", push(UCHAR_MAX); push(F_TRUE))
    QUERY_ENV("/HOLD", push(HOLD_SZ); push(F_TRUE))
    QUERY_ENV("/PAD", push(PAD_SZ); push(F_TRUE))
    QUERY_ENV("ADDRESS-UNIT-BITS", push(8 * sizeof(char)); push(F_TRUE))
    QUERY_ENV("CORE", push(F_TRUE); push(F_TRUE))
    QUERY_ENV("CORE-EXT", push(F_TRUE); push(F_TRUE))
    QUERY_ENV("FLOORED", push(F_TRUE); push(F_TRUE))
    QUERY_ENV("MAX-CHAR", push(UCHAR_MAX); push(F_TRUE))
    QUERY_ENV("MAX-D", dpush(LLONG_MAX); push(F_TRUE))
    QUERY_ENV("MAX-N", push(INT_MAX); push(F_TRUE))
    QUERY_ENV("MAX-U", push(UINT_MAX); push(F_TRUE))
    QUERY_ENV("MAX-UD", dpush(ULLONG_MAX); push(F_TRUE))
    QUERY_ENV("RETURN-STACK-CELLS", push(STACK_SZ / CELL_SZ); push(F_TRUE))
    QUERY_ENV("STACK-CELLS", push(STACK_SZ / CELL_SZ); push(F_TRUE))
    QUERY_ENV("BLOCK", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("BLOCK-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("DOUBLE", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("DOUBLE-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("EXCEPTION", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("EXCEPTION-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FACILITY", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FACILITY-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FILE", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FILE-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FLOATING", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FLOATING-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("FLOATING-STACK", push(F_FALSE))
    QUERY_ENV("MAX_FLOAT", push(F_FALSE))
    QUERY_ENV("LOCALS", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("LOCALS-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("#LOCALS", push(F_FALSE))
    QUERY_ENV("MEMORY-ALLOC", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("MEMORY-ALLOC-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("TOOLS", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("TOOLS-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("SEARCH-ORDER", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("SEARCH-ORDER-EXT", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("WORDLISTS", push(F_FALSE))
    QUERY_ENV("STRING", push(F_FALSE); push(F_TRUE))
    QUERY_ENV("STRING-EXT", push(F_FALSE); push(F_TRUE))
    else
        push(F_FALSE);
#undef QUERY_ENV
}

void die_usage(void) {
    fputs("Usage: forth [-e forth] [source [args...]]\n", stderr);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    assert(2 * CELL_SZ == sizeof(dint));
    cmd_argc = argc;
    cmd_argv = argv;       // make args global
    interactive = (argc == 1);
    init();

    cmd_argc--;
    cmd_argv++;                 // skip exec name
    while (cmd_argc > 0 && cmd_argv[0][0] == '-') {
        switch (cmd_argv[0][1]) {
        case 'e':
            if (cmd_argc == 1)
                die_usage();
            else {
                cmd_argc--;
                cmd_argv++;
                c_evaluate_sz(cmd_argv[0]);
            }
            break;
        default:
            die_usage();
        }
        cmd_argc--;
        cmd_argv++;
    }

    // get script, if any
    if (cmd_argc > 0) {
        push_file(cmd_argv[0], strlen(cmd_argv[0]));
        cmd_argc--;
        cmd_argv++;
    }

    run_forth();
}
