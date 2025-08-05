//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <iostream>
#include <fstream>
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

    const string& filename() const { return filename_; }
    ifstream* input_file() { return input_file_; }
    int source_id() const { return source_id_; }
    char* buffer() { return buffer_; }
    int size() const { return size_; }

    void open_file(const char* filename, int size);
    void open_file(const char* filename, size_t size);
    void open_file(const string& filename);

    void open_terminal();

    void set_text(const char* text, int size);
    void set_text(const char* text, size_t size);
    void set_text(const string& text);

    void set_buffer(const char* text, int size);
    void set_buffer(const char* text, size_t size);
    void set_buffer(const string& text);

    bool refill();

private:
    string filename_;           // name of the file being read
    ifstream* input_file_;      // open file
    int source_id_;             // 0: terminal, 1: file, -1: string
    char buffer_[BUFFER_SZ + 1];// input buffer +1 for BL
    int size_;                  // number of chars read
};

void f_source();
bool f_refill();
void f_accept();
int f_key();
