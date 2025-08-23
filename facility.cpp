//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "facility.h"
#include "vm.h"
#include <iostream>
using namespace std;

void f_at_xy() {
    int y = pop();
    int x = pop();
    f_at_xy(x, y);
}

void f_at_xy(int x, int y) {
    cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
}

void f_page() {
    cout << "\033[2J\033[H";
}
