//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "str.h"

void f_count(void) {
    uint addr = pop();
    uint len = cfetch(addr++);
    push(addr);
    push(len);
}
