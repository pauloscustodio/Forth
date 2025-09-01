//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include "forth.h"
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
    int read(int file_id, char* buffer, int size, Error& error_code);
    void write(int file_id, char* buffer, int size, Error& error_code);
    int read_line(int file_id, char* buffer, int size, bool& found_eof,
                  Error& error_code);
    void write_line(int file_id, char* buffer, int size, Error& error_code);
    bool seek(int file_id, udint pos, Error& error_code);
    udint tell(int file_id, Error& error_code);
    udint size(int file_id, Error& error_code);
    void resize(int file_id, udint size, Error& error_code);
    void flush(int file_id, Error& error_code);
    string filename(int file_id);

private:
    struct File {
        fstream* fs{ nullptr };
        string filename;
        ios::openmode mode;
        std::streampos last_seek;
        enum { OP_NONE, OP_READ, OP_WRITE } last_op{ OP_NONE };

        bool open_for_reading() const {
            return (mode & ios::in) != 0;
        }
        bool open_for_writing() const {
            return (mode & ios::out) != 0;
        }
    };
    vector<File> files_;

    File& get_file(int file_id);
    File& get_file_for_reading(int file_id);
    File& get_file_for_writing(int file_id);
    int next_file_id();
    bool read_line(fstream* fs, char* buffer, int size, int& num_read);
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
void f_flush_file();
void f_close_file();
void f_delete_file();
void f_rename_file();

void f_include_file();
void f_include_file(int file_id);

void f_include();

void f_included();
void f_included(const string& filename);
void f_included(const char* filename, int size);
void f_included(const char* filename, size_t size);

void f_require();
void f_required();
void f_required(const string& filename);
void f_required(const char* filename, int size);
void f_required(const char* filename, size_t size);

enum ForthFileStatus {
    FS_EXISTS = 0x00000001,
    FS_REGULAR = 0x00000002,
    FS_DIRECTORY = 0x00000004,
    FS_SYMLINK = 0x00000008,
    FS_READABLE = 0x00000010,
    FS_WRITABLE = 0x00000020,
    FS_EXECUTABLE = 0x00000040,
    FS_ERROR = 0x80000000  // high bit for error
};

void f_file_status();
void f_file_status(const string& filename);
void f_file_status(const char* filename, int size);
void f_file_status(const char* filename, size_t size);
