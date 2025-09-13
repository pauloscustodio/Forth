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

SyncStream::SyncStream(const std::string& filename, std::ios::openmode mode)
    : filename_(filename),
      file_stream_(filename, mode | std::ios::binary),
      last_op_(Operation::NONE),
      is_rw_mode_((mode & std::ios::in) && (mode & std::ios::out)),
      mode_(mode) {
}

bool SyncStream::is_open() const {
    return file_stream_.is_open();
}

bool SyncStream::good() const {
    return file_stream_.good();
}

bool SyncStream::bad() const {
    return file_stream_.bad();
}

const std::string& SyncStream::filename() const {
    return filename_;
}

uint SyncStream::read_bytes(char* buffer, uint size) {
    flush_if_needed(Operation::READ);
    sync_read_pos();
    file_stream_.read(buffer, size);
    last_op_ = Operation::READ;
    sync_write_pos();
    return static_cast<uint>(file_stream_.gcount());
}

char SyncStream::read_char() {
    flush_if_needed(Operation::READ);
    sync_read_pos();
    char c;
    file_stream_.get(c);
    last_op_ = Operation::READ;
    sync_write_pos();
    return c;
}

char SyncStream::peek_char() {
    flush_if_needed(Operation::READ);
    sync_read_pos();
    char c = file_stream_.peek();
    last_op_ = Operation::READ;
    return c;
}

uint SyncStream::read_line(char* buffer, uint size, bool& found_eof) {
    flush_if_needed(Operation::READ);
    sync_read_pos();

    char c;
    found_eof = size == 0 ? false : true;
    uint num_read = 0;
    while (num_read < size && file_stream_.get(c)) {
        found_eof = false;
        if (c == '\n') {
            break; // LF
        }
        else if (c == '\r') {
            char next = file_stream_.peek();
            if (next == '\n') {
                file_stream_.get(); // consume '\n'
            }
            break; // CR or CRLF
        }
        else {
            buffer[num_read++] = c;
        }
    }

    last_op_ = Operation::READ;
    sync_write_pos();
    return num_read;
}

void SyncStream::write_bytes(const char* buffer, uint size) {
    flush_if_needed(Operation::WRITE);
    sync_write_pos();
    file_stream_.write(buffer, size);
    last_op_ = Operation::WRITE;
    sync_read_pos();
}

void SyncStream::write_char(char c) {
    flush_if_needed(Operation::WRITE);
    sync_write_pos();
    file_stream_.put(c);
    last_op_ = Operation::WRITE;
    sync_read_pos();
}

void SyncStream::write_line(const char* buffer, uint size, EolType eol) {
    flush_if_needed(Operation::WRITE);
    sync_write_pos();

    file_stream_.write(buffer, size);

    switch (eol) {
    case EolType::LF:
        file_stream_.put('\n');
        break;
    case EolType::CRLF:
        file_stream_.put('\r');
        file_stream_.put('\n');
        break;
    case EolType::CR:
        file_stream_.put('\r');
        break;
    }

    last_op_ = Operation::WRITE;
    sync_read_pos();
}

void SyncStream::seek(udint pos, std::ios_base::seekdir dir) {
    file_stream_.clear();
    file_stream_.seekg(pos, dir);
    file_stream_.seekp(pos, dir);
}

udint SyncStream::tell() {
    if (!file_stream_.good()) {
        file_stream_.clear();
    }
    std::streampos pos = file_stream_.tellg();
    if (pos == -1) {
        file_stream_.seekg(0, std::ios::end);
        pos = file_stream_.tellg();
    }
    return static_cast<udint>(pos);
}

void SyncStream::flush() {
    file_stream_.flush();
}

void SyncStream::close() {
    file_stream_.close();
}

void SyncStream::resize(udint size) {
    std::streampos current = tell();
    close();

    std::filesystem::resize_file(filename_, size);

    // reopen with original mode except ios::trunc, add ios::in and ios::out
    // to keep current contents of file
    std::ios::openmode mode = (mode_ & ~std::ios::trunc) | std::ios::in |
                              std::ios::out;
    file_stream_.open(filename_, mode);
    last_op_ = Operation::NONE;

    seek(current);
}

void SyncStream::sync_read_pos() {
    auto pos = file_stream_.tellp();
    file_stream_.seekg(pos);
}

void SyncStream::sync_write_pos() {
    auto pos = file_stream_.tellg();
    file_stream_.seekp(pos);
}

void SyncStream::flush_if_needed(Operation next_op) {
    if (is_rw_mode_ && last_op_ == Operation::WRITE && next_op == Operation::READ) {
        file_stream_.flush();
    }
    if (is_rw_mode_ && next_op == Operation::READ) {
        file_stream_.flush();
    }
}

//-----------------------------------------------------------------------------

Files::Files() {
    files_.push_back(nullptr); // file id 0 is invalid
}

Files::~Files() {
    for (auto& file : files_) {
        delete file;
    }
}

uint Files::open(const std::string& filename, std::ios::openmode mode) {
    SyncStream* fs = new SyncStream(filename, mode);
    if (!fs->is_open()) {
        delete fs;
        return 0;
    }
    else {
        uint file_id = next_file_id();
        files_[file_id] = fs;
        return file_id;
    }
}

SyncStream* Files::get_file(uint file_id) {
    if (file_id < files_.size()) {
        return files_[file_id];
    }
    else {
        return nullptr;
    }
}

bool Files::close(uint file_id, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        if (fs->is_open()) {
            fs->close();
        }
        delete fs;
        files_[file_id] = nullptr;
        return true;
    }

    error_code = Error::CloseFileException;
    return false;
}

uint Files::read_bytes(uint file_id, char* buffer, uint size,
                       Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        uint num_read = fs->read_bytes(buffer, size);
        return num_read;
    }

    error_code = Error::ReadFileException;
    return 0;
}

void Files::write_bytes(uint file_id, const char* buffer, uint size,
                        Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        fs->write_bytes(buffer, size);
        if (!fs->bad()) {
            return;
        }
    }

    error_code = Error::WriteFileException;
}

uint Files::read_line(uint file_id, char* buffer, uint size,
                      bool& found_eof, Error& error_code) {
    found_eof = false;
    error_code = Error::None;

    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        uint num_read = fs->read_line(buffer, size, found_eof);
        if (!fs->bad()) {
            return num_read;
        }
    }

    found_eof = true;
    error_code = Error::ReadLineException;
    return 0;
}

void Files::write_line(uint file_id, char* buffer, uint size,
                       Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        fs->write_line(buffer, size);
        if (!fs->bad()) {
            return;
        }
    }

    error_code = Error::WriteLineException;
}


bool Files::seek(uint file_id, udint pos, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        fs->seek(pos);
        return true;
    }

    error_code = Error::RepositionFileException;
    return false;
}

udint Files::tell(uint file_id, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        return fs->tell();
    }

    error_code = Error::FilePositionException;
    return -1;
}

udint Files::size(uint file_id, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        std::streampos current = fs->tell();
        fs->seek(0, std::ios::end);
        std::streampos size = fs->tell();
        fs->seek(current);
        return static_cast<udint>(size);
    }

    error_code = Error::FileSizeException;
    return -1;
}

void Files::resize(uint file_id, udint size, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        fs->resize(size);
        return;
    }

    error_code = Error::ResizeException;
}

void Files::flush(uint file_id, Error& error_code) {
    error_code = Error::None;
    SyncStream* fs = get_file(file_id);
    if (fs != nullptr) {
        fs->flush();
        return;
    }

    error_code = Error::FlushFileException;
}

std::string Files::filename(uint file_id) {
    SyncStream* fs = get_file(file_id);
    return fs ? fs->filename() : "";
}

int Files::next_file_id() {
    for (uint file_id = 1; file_id < files_.size(); ++file_id) {
        if (files_[file_id] == nullptr) {
            return file_id;
        }
    }
    uint file_id = static_cast<uint>(files_.size());
    files_.push_back(nullptr);
    return file_id;
}

void f_r_o() {
    push(std::ios::in);
}

void f_w_o() {
    push(std::ios::out | std::ios::trunc);
}

void f_r_w() {
    push(std::ios::in | std::ios::out);
}

void f_bin() {
    push(pop() | std::ios::binary);
}

static void open_create(std::ios::openmode base_mode, Error error_code) {
    int mode = pop();
    uint size = pop();
    int filename_addr = pop();
    char* filename_ptr = mem_char_ptr(filename_addr, size);
    std::string filename = std::string(filename_ptr, filename_ptr + size);
    uint file_id = vm.files.open(filename,
                                 base_mode | static_cast<std::ios::openmode>(mode));
    if (file_id == 0) {
        push(0);        // file_id
        push(static_cast<int>(error_code));
    }
    else if (!vm.files.seek(file_id, 0, error_code)) {
        push(0);        // file_id
        push(static_cast<int>(error_code));
    }
    else {
        push(file_id);    // file_id
        push(0);       // no error
    }
}

void f_create_file() {
    open_create(std::ios::out | std::ios::trunc, Error::CreateFileException);
}

void f_open_file() {
    open_create(static_cast<std::ios::openmode>(0), Error::OpenFileException);
}

void f_read_file() {
    uint file_id = pop();
    uint size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    int num_read = vm.files.read_bytes(file_id, buffer, size, error_code);

    push(num_read);
    push(static_cast<int>(error_code));
}

void f_write_file() {
    uint file_id = pop();
    uint size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    vm.files.write_bytes(file_id, buffer, size, error_code);

    push(static_cast<int>(error_code));
}

void f_read_line() {
    uint file_id = pop();
    uint size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    bool found_eof = false;
    uint num_read = vm.files.read_line(file_id, buffer, size,
                                       found_eof, error_code);

    push(num_read);
    push(f_bool(!found_eof));
    push(static_cast<int>(error_code));
}

void f_write_line() {
    uint file_id = pop();
    uint size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    Error error_code = Error::None;
    vm.files.write_line(file_id, buffer, size, error_code);

    push(static_cast<int>(error_code));
}

void f_file_position() {
    uint file_id = pop();

    Error error_code = Error::None;
    udint pos = vm.files.tell(file_id, error_code);

    dpush(pos);
    push(static_cast<int>(error_code));
}

void f_reposition_file() {
    uint file_id = pop();
    udint pos = dpop();

    Error error_code = Error::None;
    vm.files.seek(file_id, pos, error_code);

    push(static_cast<int>(error_code));
}

void f_file_size() {
    uint file_id = pop();

    Error error_code = Error::None;
    udint size = vm.files.size(file_id, error_code);

    dpush(size);
    push(static_cast<int>(error_code));
}

void f_resize_file() {
    uint file_id = pop();
    udint size = dpop();

    Error error_code = Error::None;
    vm.files.resize(file_id, size, error_code);

    push(static_cast<int>(error_code));
}

void f_flush_file() {
    uint file_id = pop();

    Error error_code = Error::None;
    vm.files.flush(file_id, error_code);

    push(static_cast<int>(error_code));
}

void f_close_file() {
    uint file_id = pop();

    Error error_code = Error::None;
    vm.files.close(file_id, error_code);

    push(static_cast<int>(error_code));
}

void f_delete_file() {
    uint size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    std::string filename(filename_str, filename_str + size);

    std::error_code ec;
    bool deleted = std::filesystem::remove(filename, ec);

    push(deleted ? 0 : static_cast<int>(Error::DeleteFileException));
}

void f_rename_file() {
    uint size2 = pop();
    int filename_addr2 = pop();
    const char* filename_str2 = mem_char_ptr(filename_addr2, size2);
    std::string filename2(filename_str2, filename_str2 + size2);

    uint size1 = pop();
    int filename_addr1 = pop();
    const char* filename_str1 = mem_char_ptr(filename_addr1, size1);
    std::string filename1(filename_str1, filename_str1 + size1);

    Error error_code = Error::None;
    std::error_code ec;
    std::filesystem::rename(filename1, filename2, ec);
    if (ec) {
        error_code = Error::RenameFileException;
    }

    push(static_cast<int>(error_code));
}

void f_include_file() {
    uint file_id = pop();
    f_include_file(file_id);
}

void f_include_file(uint file_id) {
    if (file_id == 0) {
        error(Error::OpenFileException);
    }
    else {
        vm.input.save_input();
        vm.input.open_file(file_id);
        std::string filename = vm.files.filename(file_id);
        vm.included_files.insert(filename);
    }
}

void f_include() {
    uint size = 0;
    const char* filename = parse_word(size, BL);
    f_included(filename, size);
}

void f_included() {
    uint size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_included(filename_str, size);
}

void f_included(const std::string& filename) {
    uint file_id = vm.files.open(filename, std::ios::in | std::ios::binary);
    if (file_id == 0) {
        error(Error::OpenFileException, filename);
    }
    else {
        vm.input.save_input();
        vm.input.open_file(file_id);
        vm.included_files.insert(filename);
    }
}

void f_included(const char* filename, uint size) {
    std::string filename_str(filename, filename + size);
    f_included(filename_str);
}

void f_require() {
    uint size = 0;
    const char* filename = parse_word(size, BL);
    f_required(filename, size);
}

void f_required() {
    uint size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_required(filename_str, size);
}

void f_required(const std::string& filename) {
    auto it = vm.included_files.find(filename);
    if (it == vm.included_files.end()) {    // not yet included
        f_included(filename);
    }
}

void f_required(const char* filename, uint size) {
    f_required(std::string(filename, filename + size));
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
    uint size = pop();
    int filename_addr = pop();
    const char* filename_str = mem_char_ptr(filename_addr, size);
    f_file_status(filename_str, size);
}

void f_file_status(const std::string& filename) {
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

void f_file_status(const char* filename, uint size) {
    f_file_status(std::string(filename, filename + size));
}

