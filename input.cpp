//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "input.h"
#include "vm.h"
#include <cstring>
using namespace std;

void Pad::init() {
    memset(m_pad, BL, sizeof(m_pad));
}

void Input::init() {
    input_file_ = new ifstream();
    source_id_ = 0;
	memset(buffer_, BL, sizeof(buffer_));
    size_ = 0;
}

void Input::deinit() {
    delete input_file_;
}

void Input::open_file(const char* filename, int size) {
    open_file(string(filename, filename + size));
}

void Input::open_file(const char* filename, size_t size) {
    open_file(string(filename, filename + size));
}

void Input::open_file(const string& filename) {
    filename_ = filename;

    if (input_file_->is_open()) 
        input_file_->close();
    
    input_file_->open(filename, ios::binary);
    if (!input_file_->is_open()) 
        error(Error::OpenFileError, filename);

    source_id_ = 1; // file
}

void Input::open_terminal() {
    if (input_file_->is_open())
        input_file_->close();

    source_id_ = 0; // terminal
}

void Input::set_text(const char* text, int size) {
    set_buffer(text, size);
    source_id_ = -1; // string
}

void Input::set_text(const char* text, size_t size) {
    set_text(text, static_cast<int>(size));
}

void Input::set_text(const string& text) {
    set_text(text.c_str(), text.size());
}

void Input::set_buffer(const char* text, int size) {
    if (size > BUFFER_SZ)
        error(Error::BufferOverflow);

    size_ = size;
    vm.user->TO_IN = 0;
    memcpy(buffer_, text, size_);
    buffer_[size_] = BL; // BL after the string
}

void Input::set_buffer(const char* text, size_t size) {
    set_buffer(text, static_cast<int>(size));
}

void Input::set_buffer(const string& text) {
    set_buffer(text.c_str(), text.size());
}

bool Input::refill() {
    bool ok = false;
    string line;

    if (source_id_ < 0)
        return false;       // input from string
    else if (source_id_ == 0)
        ok = static_cast<bool>(std::getline(cin, line)); // input from terminal
    else
        ok = static_cast<bool>(std::getline(*input_file_, line)); // input from file

    if (line.size() > BUFFER_SZ)
        error(Error::BufferOverflow);

    size_ = static_cast<int>(line.size());
    vm.user->TO_IN = 0;
    memcpy(buffer_, line.c_str(), size_);
    buffer_[size_] = BL; // BL after the string

    return ok;
}

void f_source() {
    push(mem_addr(vm.input->buffer()));
    push(vm.input->size());
}

bool f_refill() {
    return vm.input->refill();
}

void f_accept() {
    int max_size = pop();
    int addr = pop();
    char* buffer = mem_char_ptr(addr);

    string line;
    if (std::getline(cin, line)) {
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
            line.pop_back();   // remove newline

        int size = static_cast<int>(line.size());
        if (size > max_size)
            size = max_size;
        memcpy(buffer, line.c_str(), size);
        push(size);
    }
    else
        push(0);
}

int f_key() {
    char c;
    cin >> c;
    return c;
}
