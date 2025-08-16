//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

// Portable ANS-style M* and M*/ for 32-bit cells

#pragma once

#include <cstdint>
#include <climits>
using namespace std;

void f_m_star();
void f_m_star_slash();
