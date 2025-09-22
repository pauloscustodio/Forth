//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "control.h"
#include "environment.h"
#include "errors.h"
#include "facility.h"
#include "file.h"
#include "forth.h"
#include "interp.h"
#include "kbd_input.h"
#include "locals.h"
#include "math.h"
#include "math96.h"
#include "output.h"
#include "parser.h"
#include "tools.h"
#include "vm.h"
#include <algorithm>
#include <cmath>

// define xtWORD for all words - execution token from dictionary
#define CONST(word, name, flags, value) uint xt##name = 0;
#define VAR(word, name, flags, value)   uint xt##name = 0;
#define CODE(word, name, flags, c_code) uint xt##name = 0;
#define FORTH(word, name, flags, text)  uint xt##name = 0;
#include "words.def"

// bool
int f_bool(bool f) {
    return f ? F_TRUE : F_FALSE;
}

// alignment and double cells
int aligned(int x) {
    return (x + CELL_SZ - 1) & ~(CELL_SZ - 1);
}

int dcell_lo(dint x) {
    return x & 0xffffffffLL;
}

int dcell_hi(dint x) {
    return (x >> 32) & 0xffffffffLL;
}

dint mk_dcell(int hi, int lo) {
    return
        ((static_cast<udint>(hi) & 0xffffffffLL) << 32) |
        (static_cast<udint>(lo) & 0xffffffffLL);
}

// user variables
void User::init() {
#define VAR(word, name, flags, value)   name = value;
#include "words.def"
}

void create_dictionary() {
    // first pass: define all xtWORDs, second pass: redefine all using real xtWORDs
    for (int pass = 1; pass <= 2; ++pass) {
        vm.dict.clear();

#define CONST(word, name, flags, value) xt##name = vm.dict.create(word, flags, id##name);
#define VAR(word, name, flags, value)   xt##name = vm.dict.create(word, flags, id##name);
#define CODE(word, name, flags, c_code) xt##name = vm.dict.create(word, flags, id##name);
#include "words.def"
    }

#define FORTH(word, name, flags, text)  xt##name = vm.dict.create(word, flags, idXDOCOL); compile(text);
#include "words.def"
}

void f_execute(uint xt) {
    bool do_exit = false;
    int old_ip = vm.ip;
    vm.ip = 0;
    while (true) {
        if (vm.user->TRACE) {
            Header* header = Header::header(xt);
            CString* name = header->name();
            std::cout << std::string((2 + r_depth()), '>') << BL
                      << name->to_string() << BL;
        }

        uint code = fetch(xt);
        uint body = xt + CELL_SZ;			// point to data area, if any

        switch (code) {
#define CONST(word, name, flags, value) case id##name: push(value); break;
#define VAR(word, name, flags, value)   case id##name: push(mem_addr(&vm.user->name)); break;
#define CODE(word, name, flags, c_code) case id##name: { c_code; break; }
#include "words.def"
        default:
            error(Error::InvalidMemoryAddress, std::to_string(xt));
        }

        if (vm.user->TRACE) {
            vm.stack.print_debug();
            if (vm.f_stack.depth() > 0) {
                vm.f_stack.print_debug();
            }
            std::cout << std::endl;
        }

        if (vm.ip == 0 || do_exit) {	// ip did not change, exit
            break;
        }

        xt = fetch(vm.ip);
        vm.ip += CELL_SZ;	    // else fetch next xt from ip
    }
    vm.ip = old_ip;
}
