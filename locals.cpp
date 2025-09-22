//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "locals.h"
#include "vm.h"

void clear_locals() {
    vm.last_local_index = 0;
    vm.locals.clear();
}

static void check_in_colon() {
    if (cs_ddepth() != 1) {
        error(Error::CompilerNesting);
    }

    dint pos_addr = cs_dpeek();
    if (dcell_hi(pos_addr) != POS_COLON_START) {
        error(Error::CompilerNesting);
    }
}

static void add_local(const std::string& name) {
    vm.last_local_index++;
    vm.locals[name] = vm.last_local_index;

    // compile initialization code for local
    comma(xtTOR);
}

void f_paren_local() {
    uint size = pop();
    uint addr = pop();
    const char* name = mem_char_ptr(addr, size);

    check_in_colon();  // TODO: handle locals after DOES>

    if (size != 0) {
        add_local(std::string(name, name + size));
    }
}

bool find_local(const std::string& name, uint& index) {
    auto it = vm.locals.find(name);
    if (it != vm.locals.end()) {
        index = it->second;
        return true;
    }
    else {
        index = 0;
        return false;
    }
}

bool find_local(const char* name, uint size, uint& index) {
    return find_local(std::string(name, name + size), index);
}
