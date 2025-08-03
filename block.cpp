//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "block.h"
#include "forth.h"
#include "vm.h"
using namespace std;

void f_load(int blk) {
    vm.input->push_block(blk);
    f_execute(xtINTERPRET);
    vm.input->pop_input();
}
