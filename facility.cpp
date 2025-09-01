//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "facility.h"
#include "vm.h"
#include <chrono>
#include <iostream>
#include <thread>

void f_at_xy() {
    int y = pop();
    int x = pop();
    f_at_xy(x, y);
}

void f_at_xy(int x, int y) {
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
}

void f_page() {
    std::cout << "\033[2J\033[H";
}

void f_begin_structure() {
    vm.dict->parse_create(idXDOCONST, 0);
    uint addr = vm.dict->here();     // address to store size of structure
    comma(0);                       // reserve space for size

    push(addr);                     // push address of size
    push(0);                        // initial offset is 0
}

static int field(uint offset, uint size, bool do_align) {
    if (do_align) {
        offset = aligned(offset);
    }

    vm.dict->parse_create(idXPLUS_FIELD, 0);
    comma(offset);                   // offset of field

    offset += size;

    return offset;                   // new offset
}

void f_plus_field() {
    uint size = pop();
    uint offset = pop();
    offset = field(offset, size, false);
    push(offset);                    // new offset
}

void f_xplus_field(uint body) {
    int base_addr = pop();
    uint offset = fetch(body);
    int field_addr = base_addr + offset;
    push(field_addr);
}

void f_cfield_colon() {
    uint offset = pop();
    offset = field(offset, CHAR_SZ, false);
    push(offset);                    // new offset
}

void f_field_colon() {
    uint offset = pop();
    offset = field(offset, CELL_SZ, true);
    push(offset);                    // new offset
}

void f_two_field_colon() {
    uint offset = pop();
    offset = field(offset, DCELL_SZ, true);
    push(offset);                    // new offset
}

void f_end_structure() {
    uint offset = pop();
    uint addr = pop();
    store(addr, offset); // store size of structure
}

void f_ms() {
    int milliseconds = pop();
    f_ms(milliseconds);
}

void f_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void f_time_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&t);
    push(tm->tm_sec);        // seconds after the minute - [0, 60]
    push(tm->tm_min);        // minutes after the hour - [0, 59]
    push(tm->tm_hour);       // hours since midnight - [0, 23]
    push(tm->tm_mday);       // day of the month - [1, 31]
    push(tm->tm_mon + 1);    // months since January - [0, 11]
    push(tm->tm_year + 1900);// years since 1900
}
