//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "environment.h"
#include "forth.h"
#include "str.h"
#include "vm.h"
#include <cfloat>
#include <climits>
#include <string>

int g_argc = 0;
char** g_argv = nullptr;
bool g_interactive = false;

void f_environment_q() {
    uint size = pop();
    uint addr = pop();
    char* query = mem_char_ptr(addr, size);
    f_environment_q(query, size);
}

void f_environment_q(const std::string& query) {
    if (case_insensitive_equal(query, "/COUNTED-STRING")) {
        push(MAX_CSTRING_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "/HOLD")) {
        push(PAD_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "/PAD")) {
        push(PAD_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "ADDRESS-UNIT-BITS")) {
        push(CHAR_SZ * 8);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FLOORED")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-CHAR")) {
        push(CHAR_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-D")) {
        dpush(INT64_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-N")) {
        push(INT_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-U")) {
        push(UINT_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-UD")) {
        dpush(UINT64_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "RETURN-STACK-CELLS")) {
        push(STACK_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "STACK-CELLS")) {
        push(STACK_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "CORE")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "CORE-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "BLOCK")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "BLOCK-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "DOUBLE")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "DOUBLE-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "EXCEPTION")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "EXCEPTION-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FACILITY")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FACILITY-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FILE")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FILE-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FLOATING")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FLOATING-EXT")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "FLOATING-STACK")) {
        push(STACK_SZ);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MAX-FLOAT")) {
        fpush(DBL_MAX);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "LOCALS")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "LOCALS-EXT")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MEMORY-ALLOC")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "MEMORY-ALLOC-EXT")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "TOOLS")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "TOOLS-EXT")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "SEARCH-ORDER")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "SEARCH-ORDER-EXT")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "STRING")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else if (case_insensitive_equal(query, "STRING-EXT")) {
        push(F_FALSE);
        push(F_TRUE);
    }
    else {
        push(F_FALSE);
    }
}

void f_environment_q(const char* query, uint size) {
    f_environment_q(std::string(query, query + size));
}
