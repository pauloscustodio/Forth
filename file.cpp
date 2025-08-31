//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "file.h"
#include "input.h"
#include "parser.h"
#include "vm.h"
#include <filesystem>
#include <iostream>
using namespace std;

Files::Files() {
    files_.push_back(nullptr); // file id 0 is invalid
    filenames_.push_back("");
}

Files::~Files() {
    for (auto& file : files_) {
        delete file;
    }
}

int Files::open(const string& filename, ios::openmode mode) {
    int file_id = next_file_id();
    fstream* file = new fstream(filename, mode);
    if (!file->is_open()) {
        delete file;
        return 0;
    }
    else {
        files_[file_id] = file;
        filenames_[file_id] = filename;
        return file_id;
    }
}

bool Files::close(int file_id, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr) {
            if (file->is_open()) {
                file->close();
            }
            delete file;
            files_[file_id] = nullptr;
            filenames_[file_id] = "";
            return true;
        }
    }

    error_code = Error::CloseFileException;
    return false;
}

bool Files::seek(int file_id, udint pos, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            file->seekg(pos, ios::beg);
            file->seekp(pos, ios::beg);
            return true;
        }
    }

    error_code = Error::RepositionFileException;
    return false;
}

int Files::read(int file_id, char* buffer, int size, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            file->read(buffer, size);
            if (!file->bad()) {
                sync_write_file_pos(file);
                return static_cast<int>(file->gcount());
            }
        }
    }

    error_code = Error::ReadFileException;
    return 0;
}

void Files::write(int file_id, char* buffer, int size, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            file->write(buffer, size);
            if (!file->bad()) {
                sync_read_file_pos(file);
                return;
            }
        }
    }

    error_code = Error::WriteFileException;
}

int Files::read_line(int file_id, char* buffer, int size, bool& found_eof,
                     Error& error_code) {
    found_eof = false;
    error_code = Error::None;

    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            int num_read = 0;
            bool read_ok = read_line(file, buffer, size, num_read);
            if (!file->bad()) {
                sync_write_file_pos(file);
                if (!read_ok) {
                    found_eof = true;
                }
                return num_read;
            }
        }
    }

    found_eof = true;
    error_code = Error::ReadLineException;
    return 0;
}

void Files::write_line(int file_id, char* buffer, int size, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            file->write(buffer, size);
            if (!file->bad()) {
                file->write("\n", 1);
                if (!file->bad()) {
                    sync_read_file_pos(file);
                    if (!file->bad()) {
                        return;
                    }
                }
            }
        }
    }

    error_code = Error::WriteLineException;
}

udint Files::tell(int file_id, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            if (file->eof()) {
                file->clear(); // clear eofbit and failbit
            }
            if (file->tellg() != -1) {
                return static_cast<udint>(file->tellg());
            }
            else if (file->tellp() != -1) {
                return static_cast<udint>(file->tellp());
            }
        }
    }

    error_code = Error::FilePositionException;
    return -1;
}

int Files::size(int file_id, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        streampos current = file->tellg();
        file->seekg(0, ios::end);
        streampos size = file->tellg();
        file->seekg(current); // restore position
        return static_cast<int>(size);

    }

    error_code = Error::FileSizeException;
    return -1;
}

void Files::resize(int file_id, int size, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        string filename = filenames_[file_id];
        if (!filename.empty()) {
            file->flush();
            filesystem::resize_file(filename, size);
            return;
        }
    }

    error_code = Error::ResizeException;
}

void Files::flush(int file_id, Error& error_code) {
    error_code = Error::None;
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        fstream* file = files_[file_id];
        if (file != nullptr && file->is_open()) {
            file->flush();
            return;
        }
    }

    error_code = Error::FlushFileException;
}

string Files::filename(int file_id) {
    if (file_id > 0 && file_id < static_cast<int>(files_.size())) {
        return filenames_[file_id];
    }
    else {
        return "";
    }
}

int Files::next_file_id() {
    for (int file_id = 1; file_id < static_cast<int>(files_.size()); ++file_id) {
        if (files_[file_id] == nullptr) {
            return file_id;
        }
    }
    int file_id = static_cast<int>(files_.size());
    files_.push_back(nullptr);
    filenames_.push_back("");
    return file_id;
}

bool Files::read_line(fstream* fs, char* buffer, int size, int& num_read) {
    char ch;
    num_read = 0;
    int num_eol = 0;
    while (num_read < size && fs->get(ch)) {
        if (ch == '\r') {
            ++num_eol;
            if (fs->peek() == '\n') {
                fs->get();    // consume LF after CR
                ++num_eol;
            }
            break;
        }
        else if (ch == '\n') {
            ++num_eol;
            break;
        }
        else {
            buffer[num_read++] = ch;
        }
    }
    return num_read > 0 || num_eol > 0 || num_read == size;
}

void Files::sync_write_file_pos(fstream* fs) {
    if (fs->eof()) {
        fs->clear(); // clear eofbit and failbit
    }

    streampos pos = fs->tellg();
    fs->seekp(pos); // sync write pointer to match read pointer
}

void Files::sync_read_file_pos(fstream* fs) {
    // synchronize read pointer with write pointer
    fs->flush(); // ensure data is written

    if (fs->eof()) {
        fs->clear(); // clear eofbit and failbit
    }

    // Move read pointer to match write pointer
    streampos pos = fs->tellp();
    fs->seekg(pos);

}

void f_r_o() {
    push(ios::in);
}

void f_w_o() {
    push(ios::out | ios::trunc);
}

void f_r_w() {
    push(ios::in | ios::out);
}

void f_bin() {
    push(pop() | ios::binary);
}

static void open_create(ios::openmode base_mode, Error error_code) {
    int mode = pop();
    int size = pop();
    int filename_addr = pop();
    char* filename_ptr = mem_char_ptr(filename_addr, size);
    string filename = string(filename_ptr, filename_ptr + size);
    int file_id = vm.files->open(filename,
                                 base_mode | static_cast<ios::openmode>(mode));
    if (file_id == 0) {
        push(0);        // file_id
        push(static_cast<int>(error_code));
    }
    else if (!vm.files->seek(file_id, 0, error_code)) {
        push(0);        // file_id
        push(static_cast<int>(error_code));
    }
    else {
        push(file_id);    // file_id
        push(0);       // no error
    }
}

void f_create_file() {
    open_create(ios::out | ios::trunc, Error::CreateFileException);
}

void f_open_file() {
    open_create(static_cast<ios::openmode>(0), Error::OpenFileException);
}

void f_read_file() {
    int file_id = pop();
    int size = pop();
    int addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    int num_read = vm.files->read(file_id, buffer, size, error_code);

    push(num_read);
    push(static_cast<int>(error_code));
}

void f_write_file() {
    int file_id = pop();
    int size = pop();
    int addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    vm.files->write(file_id, buffer, size, error_code);

    push(static_cast<int>(error_code));
}

void f_read_line() {
    int file_id = pop();
    int size = pop();
    int addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    bool found_eof = false;
    int num_read = vm.files->read_line(file_id, buffer, size, found_eof,
                                       error_code);

    push(num_read);
    push(f_bool(!found_eof));
    push(static_cast<int>(error_code));
}

void f_write_line() {
    int file_id = pop();
    int size = pop();
    int addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    vm.files->write_line(file_id, buffer, size, error_code);

    push(static_cast<int>(error_code));
}

void f_file_position() {
    int file_id = pop();

    Error error_code = Error::None;
    udint pos = vm.files->tell(file_id, error_code);

    dpush(pos);
    push(static_cast<int>(error_code));
}

void f_reposition_file() {
    int file_id = pop();
    udint pos = dpop();

    Error error_code = Error::None;
    vm.files->seek(file_id, pos, error_code);

    push(static_cast<int>(error_code));
}

void f_file_size() {
    int file_id = pop();

    Error error_code = Error::None;
    int size = vm.files->size(file_id, error_code);

    push(size);
    push(static_cast<int>(error_code));
}

void f_resize_file() {
    int file_id = pop();
    int size = pop();

    Error error_code = Error::None;
    vm.files->resize(file_id, size, error_code);

    push(static_cast<int>(error_code));
}

void f_flush_file() {
    int file_id = pop();

    Error error_code = Error::None;
    vm.files->flush(file_id, error_code);

    push(static_cast<int>(error_code));
}

void f_close_file() {
    int file_id = pop();

    Error error_code = Error::None;
    vm.files->close(file_id, error_code);

    push(static_cast<int>(error_code));
}

void f_delete_file() {
    int size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    string filename(filename_str, filename_str + size);

    Error error_code = Error::None;
    try {
        filesystem::remove(filename);
    }
    catch (filesystem::filesystem_error&) {
        error_code = Error::DeleteFileException;
    }
    catch (...) {
        throw;
    }

    push(static_cast<int>(error_code));
}

void f_rename_file() {
    int size2 = pop();
    int filename_addr2 = pop();
    const char* filename_str2 = mem_char_ptr(filename_addr2, size2);
    string filename2(filename_str2, filename_str2 + size2);

    int size1 = pop();
    int filename_addr1 = pop();
    const char* filename_str1 = mem_char_ptr(filename_addr1, size1);
    string filename1(filename_str1, filename_str1 + size1);

    Error error_code = Error::None;
    try {
        std::filesystem::rename(filename1, filename2);
    }
    catch (filesystem::filesystem_error&) {
        error_code = Error::RenameFileException;
    }
    catch (...) {
        throw;
    }

    push(static_cast<int>(error_code));
}

void f_include_file() {
    int file_id = pop();
    f_include_file(file_id);
}

void f_include_file(int file_id) {
    if (file_id == 0) {
        error(Error::OpenFileException);
    }
    else {
        vm.input->save_input();
        vm.input->open_file(file_id);
        string filename = vm.files->filename(file_id);
        vm.included_files.insert(filename);
    }
}

void f_include() {
    int size = 0;
    const char* filename = parse_word(size, BL);
    f_included(filename, size);
}

void f_included() {
    int size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_included(filename_str, size);
}

void f_included(const string& filename) {
    int file_id = vm.files->open(filename, ios::in | ios::binary);
    if (file_id == 0) {
        error(Error::OpenFileException, filename);
    }
    else {
        vm.input->save_input();
        vm.input->open_file(file_id);
        vm.included_files.insert(filename);
    }
}

void f_included(const char* filename, int size) {
    string filename_str(filename, filename + size);
    f_included(filename_str);
}

void f_included(const char* filename, size_t size) {
    f_included(filename, static_cast<int>(size));
}

void f_require() {
    int size = 0;
    const char* filename = parse_word(size, BL);
    f_required(filename, size);
}

void f_required() {
    int size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_required(filename_str, size);
}

void f_required(const string& filename) {
    auto it = vm.included_files.find(filename);
    if (it == vm.included_files.end()) {    // not yet included
        f_included(filename);
    }
}

void f_required(const char* filename, int size) {
    f_required(string(filename, filename + size));
}

void f_required(const char* filename, size_t size) {
    f_required(filename, static_cast<int>(size));
}

static uint32_t get_forth_file_status(const std::string& path) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::file_status status = fs::status(path, ec);

    if (ec) {
        return FS_ERROR;
    }

    uint32_t result = 0;
    if (fs::exists(status)) {
        result |= FS_EXISTS;
    }
    if (fs::is_regular_file(status)) {
        result |= FS_REGULAR;
    }
    if (fs::is_directory(status)) {
        result |= FS_DIRECTORY;
    }
    if (fs::is_symlink(status)) {
        result |= FS_SYMLINK;
    }

    fs::perms p = status.permissions();
    if ((p & fs::perms::owner_read) != fs::perms::none) {
        result |= FS_READABLE;
    }
    if ((p & fs::perms::owner_write) != fs::perms::none) {
        result |= FS_WRITABLE;
    }
    if ((p & fs::perms::owner_exec) != fs::perms::none) {
        result |= FS_EXECUTABLE;
    }

    return result;
}

void f_file_status() {
    int size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_file_status(filename_str, size);
}

void f_file_status(const string& filename) {
    uint32_t st = get_forth_file_status(filename);
    if ((st & FS_ERROR) != 0) {     // file does not exist
        push(st);
        push(static_cast<int>(Error::FileStatusException));
    }
    else {
        push(st);
        push(0);
    }
}

void f_file_status(const char* filename, int size) {
    f_file_status(string(filename, filename + size));
}

void f_file_status(const char* filename, size_t size) {
    f_file_status(filename, static_cast<int>(size));
}

