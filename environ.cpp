//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "environ.h"
#include "forth.h"
#include "str.h"
using namespace std;

int g_argc = 0;
char** g_argv = nullptr;
bool g_interactive = false;

void f_environment_q(const string& query) {
    if (case_insensitive_equal(query, "CORE")) {
        push(F_TRUE);
        push(F_TRUE);
    }
    else {
        push(F_FALSE);
    }
}

void f_environment_q(const char* query, size_t size) {
    f_environment_q(string(query, query + size));
}

void f_environment_q(const char* query, int size) {
    f_environment_q(string(query, query + size));
}
