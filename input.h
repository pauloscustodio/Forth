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
using namespace std;

class Pad {
public:
    void init();
    char* pad() { return m_pad; }
private:
    char m_pad[PAD_SZ];
};

class Input {
public:
    void init();
    void deinit();

    const string& filename() const { return *filename_; }
    ifstream* input_file() { return input_file_; }
    int source_id() const { return source_id_; }
    const char* buffer() const { return buffer_; }

    void open_file(const char* filename, int size);
    void open_file(const char* filename, size_t size);
    void open_file(const string& filename);

    void open_terminal();

    void set_text(const char* text, int size);
    void set_text(const char* text, size_t size);

    void set_block(Block* block);

    void set_tib(const char* text, int size);
    void set_tib(const char* text, size_t size);
    void set_tib(const string& text);

    bool refill();

    void save_input();
    bool restore_input();

    void save_input_for_query();
    bool restore_input_if_query();

    int input_level() const;
    void restore_input(int level);

private:
    string* filename_;          // name of the file being read
    ifstream* input_file_;      // open file
    int source_id_;             // 0: terminal, 1: file, -1: string
    const char* buffer_;        // current input buffer, tib_ or the string supplied to EVALUATE
    char tib_[BUFFER_SZ + 1];   // input buffer +1 for BL
    int num_query_;             // number of times f_query was called and save_input() called within

    struct SaveInput {
        string filename;
        bool is_open;
        streampos fpos;
        int source_id;
        int blk;
        string tib;
        const char* buffer;
        int nr_in;
        int to_in;
    };

    vector<SaveInput>* input_stack_;    // stack of saved inputs
};

void f_source();
void f_tib();
bool f_refill();
void f_accept();
int f_key();
void f_expect();
void f_query();
void f_save_input();
bool f_restore_input();
