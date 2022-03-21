//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

int f_mod(int a, int b);
int f_div(int a, int b);
void f_div_mod(void);
dint f_dmod(dint a, dint b);
dint f_ddiv(dint a, dint b);
void f_ddiv_mod(void);
void f_fm_div_mod(void);
void f_sm_div_rem(void);
void f_um_div_mod(void);
void f_mul_div_mod(void);
void f_mul_div(void);
int f_abs(int a);
int f_max(int a, int b);
int f_min(int a, int b);
dint f_dabs(dint a);
dint f_dmax(dint a, dint b);
dint f_dmin(dint a, dint b);
bool c_within(int x, int a, int b);
void f_within(void);
