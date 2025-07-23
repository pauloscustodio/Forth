//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

// alignment and double cells
int aligned(int x);
int dcell_lo(dint x);
int dcell_hi(dint x);
int dcell(int hi, int lo);

dint cDABS(dint a);
