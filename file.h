//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Files {
public:
    Files();
    virtual ~Files();

    // returns file id, 0 on failure
    int open(const string& filename, ios::openmode mode);
    bool close(int file_id, Error& error_code);
    bool seek(int file_id, int pos, Error& error_code);
    int read(int file_id, char* buffer, int size, Error& error_code);
    void write(int file_id, char* buffer, int size, Error& error_code);
    int read_line(int file_id, char* buffer, int size, bool& found_eof,
                  Error& error_code);
    void write_line(int file_id, char* buffer, int size, Error& error_code);
    int tell(int file_id, Error& error_code);
    int size(int file_id, Error& error_code);
    void resize(int file_id, int size, Error& error_code);

private:
    vector<fstream*> files_;
    vector<string>   filenames_;

    int next_file_id();
    bool read_line(fstream* fs, char* buffer, int size, int& num_read);
    void sync_write_file_pos(fstream* fs);
    void sync_read_file_pos(fstream* fs);
};

void f_r_o();
void f_w_o();
void f_r_w();
void f_bin();

void f_create_file();
void f_open_file();
void f_read_file();
void f_write_file();
void f_read_line();
void f_write_line();
void f_file_position();
void f_reposition_file();
void f_file_size();
void f_resize_file();
void f_close_file();
void f_delete_file();

void f_include_file();
void f_include_file(int file_id);

void f_included();
void f_included(const string& filename);
void f_included(const char* filename, int size);
void f_included(const char* filename, size_t size);
