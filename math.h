//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

int f_mod(int a, int b);
int f_div(int a, int b);
double f_div(double a, double b);
void f_div_mod();
dint f_dmod(dint a, dint b);
dint f_ddiv(dint a, dint b);
void f_ddiv_mod();
void f_fm_div_mod();
void f_sm_div_rem();
void f_um_div_mod();
void f_mul_div_mod();
void f_mul_div();
bool within(uint x, uint a, uint b);
void f_within();
void f_um_mult();
void f_m_plus();
void d_to_f();
void f_to_d();
void s_to_f();
void f_to_s();

struct RepresentResult {
    std::string digits;
    int exponent;
    bool is_negative;
};

RepresentResult f_represent(double x, int significant_digits);
void f_represent();

bool f_f_tilde();
bool f_f_tilde(double a, double b, double tolerance);
