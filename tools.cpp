//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "tools.h"
#include "vm.h"
#include <iomanip>
#include <iostream>
#include <cassert>
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
        cout << endl << hex << setfill('0') << setw(8) << p << BL << BL;
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

static void dump_colon_definition(int body) {
    int ptr = body;
    while (true) {
        cout << BL << BL;
        int xt = fetch(ptr); ptr += CELL_SZ;
        print_number(ptr, 6);
        cout << BL << "(";
        print_number(xt, 6);
        cout << ")" << BL << ":" << BL << BL;

        Header* header = Header::header(xt);
        cout << header->name()->to_string() << BL;
        if (xt == xtXSEMICOLON) {
            cout << endl << ";" << endl;
            break;
        }
        else if (xt == xtXLITERAL) {
            int value = fetch(ptr); ptr += CELL_SZ;
            print_number(value);
        }
        else if (xt == xtX2LITERAL) {
            dint value = dfetch(ptr); ptr += DCELL_SZ;
            print_number(value);
        }
        else if (xt == xtBRANCH || xt == xtZBRANCH) {
            int dist = fetch(ptr);
            ptr += CELL_SZ;
            int target = ptr + dist;
            print_number(target);
        }
        else if (xt == xtXDOT_QUOTE || xt == xtXS_QUOTE || xt == xtXABORT_QUOTE) {
            int str_addr = fetch(ptr); ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                mem_char_ptr(str_addr));
            print_string(message->str(), message->size());
            cout << '"';
        }
        else if (xt == xtXC_QUOTE) {
            int str_addr = fetch(ptr); ptr += CELL_SZ;
            const CString* message = reinterpret_cast<const CString*>(
                mem_char_ptr(str_addr));
            print_string(message->str(), message->size());
            cout << '"';
        }

        cout << endl;
    }
}

void f_see() {
    Header* header = vm.dict->parse_find_existing_word();
    assert(header != nullptr);
    int xt = header->xt();
    int body = xt + CELL_SZ;
    int code = header->code;
    switch (code) {
    case idXDOCOL:
        cout << endl << ": " << header->name()->to_string() << endl;
        dump_colon_definition(body);
        break;
    default:
        error(Error::InvalidWordXT);
    }
}
