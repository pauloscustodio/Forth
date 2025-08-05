//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

using namespace std;

void f_if();
void f_else();
void f_then();

void f_do();
void f_xdo();

void f_query_do();
void f_xquery_do();

void f_loop();
void f_xloop();

void f_plus_loop();
void f_xplus_loop();

void f_leave();
void f_xleave();

void f_unloop();
void f_xunloop();

void f_begin();
void f_again();
void f_until();
void f_while();
void f_repeat();
void f_recurse();

void f_case();
void f_of();
void f_xof();
void f_endof();
void f_endcase();
