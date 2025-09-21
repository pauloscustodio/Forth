//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

void init_console_output();

void f_at_xy();
void f_at_xy(int x, int y);
void f_page();

void f_begin_structure();
void f_plus_field();
void f_xplus_field(uint body);
void f_cfield_colon();
void f_field_colon();
void f_two_field_colon();
void f_f_field_colon();
void f_end_structure();

void f_ms();
void f_ms(int milliseconds);
void f_time_date();
