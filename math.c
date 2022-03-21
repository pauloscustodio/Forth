//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2022
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "math.h"
#include "io.h"
#include "stack.h"
#include "errors.h"

int f_mod(int a, int b) {
    if (b == 0)
        error(ErrorDivisionByZero);

    if (b < 0)
        return -f_mod(-a, -b);
    else {
        int ret = a % b;
        if (ret < 0)
            ret += b;
        return ret;
    }
}

int f_div(int a, int b) {
    int rem = f_mod(a, b);
    int quot = (a - rem) / b;
    return quot;
}

void f_div_mod(void) {
    int b = pop();
    int a = pop();
    push(f_mod(a, b));
    push(f_div(a, b));
}

dint f_dmod(dint a, dint b) {
    if (b == 0) {
        error(ErrorDivisionByZero);
        return 0;   // not reached
    }
    else {
        if (b < 0)
            return -f_dmod(-a, -b);
        else {
            dint ret = a % b;
            if (ret < 0)
                ret += b;
            return ret;
        }
    }
}

dint f_ddiv(dint a, dint b) {
    dint rem = f_dmod(a, b);
    dint quot = (a - rem) / b;
    return quot;
}

void f_ddiv_mod(void) {
    dint b = dpop();
    dint a = dpop();
    dpush(f_dmod(a, b));
    dpush(f_ddiv(a, b));
}

void f_fm_div_mod(void) {
    dint n = (dint)pop();
    dint d = dpop();
    push((int)f_dmod(d, n));
    push((int)f_ddiv(d, n));
}

void f_sm_div_rem(void) {
    dint n = (dint)pop();
    dint d = dpop();
    if (n == 0)
        error(ErrorDivisionByZero);
    else {
        push((int)(d % n));
        push((int)(d / n));
    }
}

void f_um_div_mod(void) {
    udint n = (udint)pop();
    udint d = dpop();
    if (n == 0)
        error(ErrorDivisionByZero);
    else {
        push((int)(d % n));
        push((int)(d / n));
    }
}

static void mul_div_mod(int a, int b, int c, int* quot, int* rem) {
    dint prod = (dint)a * (dint)b;
    *quot = (int)f_ddiv(prod, c);
    *rem = (int)f_dmod(prod, c);
}

void f_mul_div_mod(void) {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, &quot, &rem);
    push(rem);
    push(quot);
}

void f_mul_div(void) {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, &quot, &rem);
    push(quot);
}

int f_abs(int a) {
    return (a < 0) ? -a : a;
}

int f_max(int a, int b) {
    return a > b ? a : b;
}

int f_min(int a, int b) {
    return a < b ? a : b;
}

dint f_dabs(dint a) {
    return (a < 0) ? -a : a;
}

dint f_dmax(dint a, dint b) {
    return a > b ? a : b;
}

dint f_dmin(dint a, dint b) {
    return a < b ? a : b;
}

bool c_within(int x, int a, int b) {
    if (a > b)
        return c_within(x, b, a);
    else if (x >= a && x < b)
        return true;
    else
        return false;
}

void f_within(void) {
    int b = pop();
    int a = pop();
    int x = pop();
    push(F_BOOL(c_within(x, a, b)));
}
