//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "tools.h"
#include "vm.h"
#include <iomanip>
#include <iostream>
using namespace std;

void f_dump() {
    int size = pop();
    int addr = pop();
    const char* mem = mem_char_ptr(addr);
    f_dump(mem, size);
}

void f_dump(const char* mem, int size) {
    int addr = mem_addr(mem);
    int addr_lo = addr & ~0xF;
    int addr_hi = (addr + size + 15) & ~0xF;
    for (int p = addr_lo; p < addr_hi; p += 16) {
        cout << endl << hex << setfill('0') << setw(4) << p << BL << BL;
        for (int q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size)
                cout << BL << BL << BL;
            else
                cout << hex << setfill('0') << setw(2) << cfetch(q) << BL;
        }
        cout << BL << BL;
        for (int q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size) {
                cout << BL;
            }
            else {
                char c = cfetch(q);
                cout << (isprint(c) ? c : '.');
            }
        }
    }
    cout << endl << dec << setfill(' ') << setw(0);
}
