//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "math.h"
#include "stack.h"
#include "vm.h"
using namespace std;

int f_mod(int a, int b) {
    if (b == 0) {
        error(Error::DivisionByZero);
        return 0; // not reached
    }
    else if (b < 0) {
        return -f_mod(-a, -b);
    }
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

void f_div_mod() {
    int b = pop();
    int a = pop();
    push(f_mod(a, b));
    push(f_div(a, b));
}

dint f_dmod(dint a, dint b) {
    if (b == 0) {
        error(Error::DivisionByZero);
        return 0;   // not reached
    }
    else if (b < 0) {
        return -f_dmod(-a, -b);
    }
    else {
        dint ret = a % b;
        if (ret < 0)
            ret += b;
        return ret;
    }
}

dint f_ddiv(dint a, dint b) {
    dint rem = f_dmod(a, b);
    dint quot = (a - rem) / b;
    return quot;
}

void f_ddiv_mod() {
    dint b = dpop();
    dint a = dpop();
    dpush(f_dmod(a, b));
    dpush(f_ddiv(a, b));
}

void f_fm_div_mod() {
    dint n = (dint)pop();
    dint d = dpop();
    push(static_cast<int>(f_dmod(d, n)));
    push(static_cast<int>(f_ddiv(d, n)));
}

void f_sm_div_rem() {
    dint n = static_cast<dint>(pop());
    dint d = dpop();
    if (n == 0) {
        error(Error::DivisionByZero);
    }
    else {
        push(static_cast<int>(d % n));
        push(static_cast<int>(d / n));
    }
}

void f_um_div_mod() {
    udint n = static_cast<uint>(pop());
    udint d = dpop();
    if (n == 0) {
        error(Error::DivisionByZero);
    }
    else {
        push(static_cast<int>(d % n));
        push(static_cast<int>(d / n));
    }
}

static void mul_div_mod(int a, int b, int c, int& quot, int& rem) {
    dint prod = (dint)a * (dint)b;
    quot = static_cast<int>(f_ddiv(prod, c));
    rem = static_cast<int>(f_dmod(prod, c));
}

void f_mul_div_mod() {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, quot, rem);
    push(rem);
    push(quot);
}

void f_mul_div() {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, quot, rem);
    push(quot);
}

int f_abs(int a) {
    return a < 0 ? -a : a;
}

int f_max(int a, int b) {
    return a > b ? a : b;
}

int f_min(int a, int b) {
    return a < b ? a : b;
}

dint f_dabs(dint a) {
    return a < 0 ? -a : a;
}

dint f_dmax(dint a, dint b) {
    return a > b ? a : b;
}

dint f_dmin(dint a, dint b) {
    return a < b ? a : b;
}

bool within(uint x, uint lo, uint hi) {
    // implement the same logic as Forth's WITHIN word
    return (x - lo) < (hi - lo);
}

void f_within() {
    uint hi = pop();
    uint lo = pop();
    uint x = pop();
    push(f_bool(within(x, lo, hi)));
}

void f_um_mult() {
    udint b = static_cast<uint>(pop()); 
    udint a = static_cast<uint>(pop()); 
    udint result = a * b;
    dpush(result);
}

void f_m_star_slash() {
    dint n2 = pop();
    dint n1 = pop();
    dint d1 = dpop();

    if (n2 == 0)
        error(Error::DivisionByZero);

    dpush(f_ddiv(d1 * n1, n2));
}

void f_m_plus() {
    dint n = pop();
    dint d = dpop();
    dpush(d + n);
}

