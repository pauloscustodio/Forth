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

void f_begin_structure() {
    vm.dict->parse_create(idXDOCONST, 0);
    int addr = vm.dict->here();     // address to store size of structure
    comma(0);                       // reserve space for size

    push(addr);                     // push address of size
    push(0);                        // initial offset is 0
}

void f_plus_field() {
    int size = pop();
    int offset = pop();

    vm.dict->parse_create(idXPLUS_FIELD, 0);
    comma(offset);                   // offset of field

    offset += size;
    push(offset);                    // new offset
}

void f_xplus_field(int body) {
    int base_addr = pop();
    int offset = fetch(body);
    int field_addr = base_addr + offset;
    push(field_addr);
}

void f_end_structure() {
    int offset = pop();
    int addr = pop();
    store(addr, offset); // store size of structure
}
