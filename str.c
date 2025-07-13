//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "str.h"
#include "dict.h"
#include "errors.h"
#include "interp.h"
#include "memory.h"
#include "parser.h"
#include "stack.h"
#include <stdio.h>

void f_count(void) {
    int addr = pop();
    int len = cfetch(addr++);
    push(addr);
    push(len);
}

void f_dot_quote(void) {
    int len = 0;
    const char* message = c_parse('"', &len);
    if (user->STATE == STATE_COMPILE) {
        comma(xtXDOT_QUOTE);
        long_str_comma(message, len);
    }
    else
        printf("%.*s", len, message);
}

void f_xdot_quote(void) {
    int len = 0;
    const char* message = long_str_fetch(ip, &len);
    ip += long_str_size(len);
    printf("%.*s", len, message);
}

void f_s_quote(void) {
    int len = 0;
    const char* message = c_parse('"', &len);
    if (user->STATE == STATE_COMPILE) {
        comma(xtXS_QUOTE);
        long_str_comma(message, len);
    }
    else {
        push(message - mem);
        push(len);
    }
}

void f_xs_quote(void) {
    int len = 0;
    const char* message = long_str_fetch(ip, &len);
    ip += long_str_size(len);
    push(message - mem);
    push(len);
}

void f_c_quote(void) {
    int len = 0;
    const char* message = c_parse('"', &len);
    if (len > F_COUNTED_STR_MASK)
        error_arg(ErrorStringTooLong, fstr_to_cstr(message, len));
    if (user->STATE == STATE_COMPILE) {
        comma(xtXC_QUOTE);
        counted_str_comma(message, len);
    }
    else
        push(fstr_to_counted_str(message, len));
}

void f_xc_quote(void) {
    int len = cfetch(ip);
    int cstr = ip;
    ip += counted_str_size(len);
    push(cstr);
}

void f_dot_paren(void) {
    int len = 0;
    const char* message = c_parse(')', &len);
    printf("%.*s", len, message);
}
