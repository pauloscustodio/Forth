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
    enum class operation { none, read, write };
    enum class eol_type { lf, crlf, cr };

    explicit SyncStream(const std::string& filename, std::ios::openmode mode)
        : file_stream(filename, mode | std::ios::binary),
        last_op(operation::none),
        is_rw_mode((mode& std::ios::in) && (mode& std::ios::out)) {
        assert(file_stream.is_open());
    }

    std::vector<char> read_bytes(size_t count) {
        flush_if_needed(operation::read);
        sync_read_pos();
        std::vector<char> buffer(count);
        file_stream.read(buffer.data(), count);
        last_op = operation::read;
        sync_write_pos();
        return buffer;
    }

    char read_char() {
        flush_if_needed(operation::read);
        sync_read_pos();
        char c;
        file_stream.get(c);
        last_op = operation::read;
        sync_write_pos();
        return c;
    }

    char peek_char() {
        flush_if_needed(operation::read);
        sync_read_pos();
        char c = file_stream.peek();
        last_op = operation::read;
        return c;
    }

    std::string read_line() {
        flush_if_needed(operation::read);
        sync_read_pos();

        std::string line;
        char c;

        while (file_stream.get(c)) {
            if (c == '\n') {
                break; // LF
            }
            else if (c == '\r') {
                char next = file_stream.peek();
                if (next == '\n') {
                    file_stream.get(); // consume '\n'
                }
                break; // CR or CRLF
            }
            else {
                line += c;
            }
        }

        last_op = operation::read;
        sync_write_pos();
        return line;
    }

    void write_bytes(const std::vector<char>& data) {
        flush_if_needed(operation::write);
        sync_write_pos();
        file_stream.write(data.data(), data.size());
        last_op = operation::write;
        sync_read_pos();
    }

    void write_char(char c) {
        flush_if_needed(operation::write);
        sync_write_pos();
        file_stream.put(c);
        last_op = operation::write;
        sync_read_pos();
    }

    void write_line(const std::string& line, eol_type eol = eol_type::lf) {
        flush_if_needed(operation::write);
        sync_write_pos();

        file_stream.write(line.c_str(), line.size());

        switch (eol) {
        case eol_type::lf:
            file_stream.put('\n');
            break;
        case eol_type::crlf:
            file_stream.put('\r');
            file_stream.put('\n');
            break;
        case eol_type::cr:
            file_stream.put('\r');
            break;
        }

        last_op = operation::write;
        sync_read_pos();
    }

    void seek(std::streampos pos) {
        file_stream.clear();
        file_stream.seekg(pos);
        file_stream.seekp(pos);
    }

    std::streampos tell() {
        if (!file_stream.good()) {
            file_stream.clear();
        }
        std::streampos pos = file_stream.tellg();
        if (pos == -1) {
            file_stream.seekg(0, std::ios::end);
            pos = file_stream.tellg();
        }
        return pos;
    }

    void flush() { file_stream.flush(); }
    void close() { file_stream.close(); }
    bool is_open() const { return file_stream.is_open(); }

private:
    std::fstream file_stream;
    operation last_op;
    bool is_rw_mode;

    void sync_read_pos() {
        auto pos = file_stream.tellp();
        file_stream.seekg(pos);
    }

    void sync_write_pos() {
        auto pos = file_stream.tellg();
        file_stream.seekp(pos);
    }

    void flush_if_needed(operation next_op) {
        if (is_rw_mode && last_op == operation::write && next_op == operation::read) {
            file_stream.flush();
        }
        if (is_rw_mode && next_op == operation::read) {
            file_stream.flush();
        }
    }
};

class Files {
public:
    Files();
    virtual ~Files();

    // returns file id, 0 on failure
    int open(const std::string& filename, std::ios::openmode mode);
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
    std::string filename(int file_id);

private:
    struct File {
        std::fstream* fs{ nullptr };
        std::string filename;
        std::ios::openmode mode;
        std::streampos fpos;
        enum { OP_NONE, OP_READ, OP_WRITE } last_op{ OP_NONE };

        bool open_for_reading() const {
            return (mode & std::ios::in) != 0;
        }
        bool open_for_writing() const {
            return (mode & std::ios::out) != 0;
        }
    };
    std::vector<File> files_;

    File& get_file(int file_id);
    File& get_file_for_reading(int file_id);
    File& get_file_for_writing(int file_id);
    int next_file_id();
    bool read_line(std::fstream* fs, char* buffer, int size, int& num_read);
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
void f_included(const std::string& filename);
void f_included(const char* filename, int size);
void f_included(const char* filename, size_t size);

void f_require();
void f_required();
void f_required(const std::string& filename);
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
void f_file_status(const std::string& filename);
void f_file_status(const char* filename, int size);
void f_file_status(const char* filename, size_t size);
