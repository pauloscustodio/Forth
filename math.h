//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

int f_mod(int a, int b);
int f_div(int a, int b);
void f_div_mod();
dint f_dmod(dint a, dint b);
dint f_ddiv(dint a, dint b);
void f_ddiv_mod();
void f_fm_div_mod();
void f_sm_div_rem();
void f_um_div_mod();
void f_mul_div_mod();
void f_mul_div();
int f_abs(int a);
int f_max(int a, int b);
int f_min(int a, int b);
dint f_dabs(dint a);
dint f_dmax(dint a, dint b);
dint f_dmin(dint a, dint b);
bool within(uint x, uint a, uint b);
void f_within();
void f_um_mult();
