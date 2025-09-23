//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "block.h"
#include "forth.h"
#include <fstream>
#include <iostream>
#include <vector>

class Pad {
public:
    void init();

// pad buffer pointer by vm.pad_data
};

class Input {
public:
    void init();

    int source_id() const;
    const char* buffer() const;

    void open_file(const std::string& filename);
    void open_file(int source_id);

    void open_terminal();

    void set_text(const char* text, uint size);

    void set_block(Block* block);

    void set_tib(const char* text, uint size);
    void set_tib(const std::string& text);

    bool refill();

    void save_input();
    bool restore_input();

    void save_input_for_query();
    bool restore_input_if_query();

    int input_level() const;
    void restore_input(int level);

private:
    int source_id_;         // 0: terminal, >=1: file, -1: string
    int num_query_;         // number of times f_query was called and save_input() called within

    // data in vm.tib_data and vm.tib_ptr

    struct SaveInput {
        int source_id;
        udint fpos;
        int blk;
        std::string tib;
        const char* tib_ptr;
        int nr_in;
        int to_in;
    };

    std::vector<SaveInput> input_stack_;    // stack of saved inputs
};

void f_source();
void f_tib();
bool f_refill();
void f_accept();
void f_expect();
void f_query();
void f_save_input();
bool f_restore_input();
