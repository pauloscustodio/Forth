//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "double.h"
#include "stack.h"
#include "errors.h"

void f_m_star_slash() {
    dint n2 = pop();
    dint n1 = pop();
    dint d1 = dpop();

    if (n2 == 0)
        error(ErrorDivisionByZero);

    dpush(d1 * n1 / n2);
}

void f_m_plus() {
    dint n = pop();
    dint d = dpop();
    dpush(d + n);
}
