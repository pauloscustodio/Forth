//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "locals.h"
#include "parser.h"
#include "vm.h"
#include <string>
#include <vector>

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

void f_paren_local() {
    uint size = pop();
    uint addr = pop();
    const char* name = mem_char_ptr(addr, size);

    if (size != 0) {
        f_paren_local(std::string(name, name + size));
    }
}

void f_paren_local(const char* name, uint size) {
    f_paren_local(std::string(name, name + size));
}

void f_paren_local(const std::string& name) {
    check_in_colon();

    uint index;
    if (find_local(name, index)) {
        error(Error::DuplicateDefinition, name);
    }

    vm.last_local_index++;
    vm.locals[name] = vm.last_local_index;

    // compile initialization code for local
    comma(xtTOR);
}

void f_locals_bar() {
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);
        if (size == 0 || (size == 1 && word[0] == '|')) {
            break;  // end of locals
        }
        f_paren_local(word, size);
    }
}

void f_locals_bracket() {
    int state = 0;  // collecting locals to be initialized from stack
    std::vector<std::string> init_locals;
    std::vector<std::string> uninit_locals;
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);

        if (size == 0) {
            break;    // end of locals
        }

        std::string name(word, word + size);
        if (name == ":}") {
            break;    // end of locals
        }
        else if (name == "|") {
            state = 1;    // collecting uninitialized locals
        }
        else if (name == "--") {
            state = 2;    // collecting and discarding output params
        }
        else if (state == 0) {
            init_locals.push_back(name);  // initialized locals
        }
        else if (state == 1) {
            uninit_locals.push_back(name);  // uninitialized locals
        }
        else if (state == 2) {
            // discard output params
        }
    }

    // build output code
    while (!init_locals.empty()) {
        f_paren_local(init_locals.back());
        init_locals.pop_back();
    }
    while (!uninit_locals.empty()) {
        comma(xtXLITERAL);
        comma(0);  // initialize to 0
        f_paren_local(uninit_locals.back());
        uninit_locals.pop_back();
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
