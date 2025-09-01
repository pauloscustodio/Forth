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
#include <cassert>

class SyncStream {
public:
    enum class Operation { NONE, READ, WRITE };
    enum class EolType { LF, CRLF, CR };

    explicit SyncStream(const std::string& filename, std::ios::openmode mode);
    bool is_open() const;
    bool good() const;
    bool bad() const;
    const std::string& filename() const;
    uint read_bytes(char* buffer, uint size);
    char read_char();
    char peek_char();
    uint read_line(char* buffer, uint size, bool& found_eof);
    void write_bytes(const char* buffer, uint size);
    void write_char(char c);
    void write_line(const char* buffer, uint size, EolType eol = EolType::LF);
    void seek(udint pos, std::ios_base::seekdir dir = std::ios_base::beg);
    udint tell();
    void flush();
    void close();
    void resize(udint size);

private:
    std::string filename_;
    std::fstream file_stream_;
    Operation last_op_;
    bool is_rw_mode_;
    std::ios::openmode mode_;

    void sync_read_pos();
    void sync_write_pos();
    void flush_if_needed(Operation next_op);
};

class Files {
public:
    Files();
    virtual ~Files();

    // returns file id, 0 on failure
    uint open(const std::string& filename, std::ios::openmode mode);
    bool close(uint file_id, Error& error_code);
    uint read_bytes(uint file_id, char* buffer, uint size, Error& error_code);
    void write_bytes(uint file_id, const char* buffer, uint size,
                     Error& error_code);
    uint read_line(uint file_id, char* buffer, uint size,
                   bool& found_eof, Error& error_code);
    void write_line(uint file_id, char* buffer, uint size, Error& error_code);
    bool seek(uint file_id, udint pos, Error& error_code);
    udint tell(uint file_id, Error& error_code);
    udint size(uint file_id, Error& error_code);
    void resize(uint file_id, udint size, Error& error_code);
    void flush(uint file_id, Error& error_code);
    std::string filename(uint file_id);

private:
    std::vector<SyncStream*> files_;

    SyncStream* get_file(uint file_id);
    int next_file_id();
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
void f_include_file(uint file_id);

void f_include();

void f_included();
void f_included(const std::string& filename);
void f_included(const char* filename, uint size);

void f_require();
void f_required();
void f_required(const std::string& filename);
void f_required(const char* filename, uint size);

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
void f_file_status(const std::string& filename);
void f_file_status(const char* filename, uint size);
