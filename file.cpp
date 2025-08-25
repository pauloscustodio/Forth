//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "file.h"
#include "input.h"
#include "vm.h"
using namespace std;

void f_included() {
    int size = pop();
    int filename_addr = pop();
    const char* filename = mem_char_ptr(filename_addr);
    f_included(filename, size);
}

void f_included(const string& filename) {
    f_included(filename.c_str(), filename.size());
}

void f_included(const char* filename, int size) {
    vm.input->save_input();
    vm.input->open_file(filename, size);
}

void f_included(const char* filename, size_t size) {
    f_included(filename, static_cast<int>(size));
}
