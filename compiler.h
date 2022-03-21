//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2022
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

void f_if(void);
void f_else(void);
void f_then(void);

void f_do(void);
void f_xdo(void);

void f_query_do(void);
void f_xquery_do(void);

void f_loop(void);
void f_xloop(void);

void f_plus_loop(void);
void f_xplus_loop(void);

void f_leave(void);
void f_xleave(void);

void f_unloop(void);
void f_xunloop(void);

void f_begin(void);
void f_again(void);
void f_until(void);
void f_while(void);
void f_repeat(void);

void f_case(void);
void f_of(void);
void f_xof(void);
void f_endof(void);
void f_endcase(void);
