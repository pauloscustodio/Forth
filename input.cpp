//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "block.h"
#include "errors.h"
#include "input.h"
#include "vm.h"
#include <cstring>

void Pad::init() {
    memset(vm.pad_data, BL, PAD_SZ);
}

//-----------------------------------------------------------------------------

void Input::init() {
    source_id_ = 0;
    vm.tib_ptr = vm.tib_data;
    memset(vm.tib_data, BL, TIB_SZ);
    num_query_ = 0;
}

int Input::source_id() const {
    return source_id_;
}

const char* Input::buffer() const {
    return vm.tib_ptr;
}

void Input::open_file(const std::string& filename) {
    source_id_ = vm.files->open(filename, std::ios::in | std::ios::binary);
    if (source_id_ == 0) {
        error(Error::OpenFileException, filename);
    }
    set_tib("", 0);
}

void Input::open_file(int source_id) {
    source_id_ = source_id;
    set_tib("", 0);
}

void Input::open_terminal() {
    source_id_ = 0; // terminal
    set_tib("", 0);
}

void Input::set_text(const char* text, uint size) {
    source_id_ = -1; // string
    vm.user->NR_IN = size;
    vm.user->TO_IN = 0;
    vm.tib_ptr = text;
}

void Input::set_block(Block* block) {
    vm.user->BLK = block->blk;
    vm.user->NR_IN = BLOCK_SZ;
    vm.user->TO_IN = 0;
    vm.tib_ptr = block->data();
    source_id_ = 0;
}

void Input::set_tib(const char* text, uint size) {
    if (size > BUFFER_SZ) {
        error(Error::InputBufferOverflow);
    }
    else {
        vm.user->NR_IN = size;
        vm.user->TO_IN = 0;
        memcpy(vm.tib_data, text, size);
        vm.tib_data[size] = BL; // BL after the string
        vm.tib_ptr = vm.tib_data;
    }
}

void Input::set_tib(const std::string& text) {
    set_tib(text.c_str(), static_cast<uint>(text.size()));
}

bool Input::refill() {
    bool ok = false;
    std::string line;

    if (source_id_ < 0) {               // input from string
        return false;
    }
    else if (vm.user->BLK > 0) {        // input from block
        if (vm.user->BLK + 1 >= vm.blocks.num_blocks()) {
            return false;
        }
        else {
            ++vm.user->BLK;
            Block* block = vm.blocks.f_block(vm.user->BLK);
            set_block(block);

            if (vm.user->TRACE) {
                std::cout << std::endl << "> ";
                print_string(block->data(), BLOCK_SZ);
                std::cout << std::endl;
            }

            return true;
        }
    }
    else if (source_id_ == 0) {         // input from terminal
        ok = static_cast<bool>(std::getline(std::cin, line));
        if (line.size() > BUFFER_SZ) {
            error(Error::InputBufferOverflow);
        }

        vm.user->NR_IN = static_cast<int>(line.size());
        vm.user->TO_IN = 0;
        memcpy(vm.tib_data, line.c_str(), line.size());
        vm.tib_data[vm.user->NR_IN] = BL; // BL after the string
        vm.tib_ptr = vm.tib_data;
    }
    else {                              // input from file
        Error error_code = Error::None;
        bool found_eof = false;
        uint num_read = vm.files->read_line(source_id_, vm.tib_data, BUFFER_SZ,
                                            found_eof, error_code);
        ok = num_read > 0 || !found_eof;

        vm.user->NR_IN = num_read;
        vm.user->TO_IN = 0;
        vm.tib_data[num_read] = BL; // BL after the string
        vm.tib_ptr = vm.tib_data;

        line = std::string(vm.tib_data, vm.tib_data + num_read);
    }

    if (ok && vm.user->TRACE) {
        std::cout << std::endl << "> " << line << std::endl;
    }

    return ok;
}

// save input context
void Input::save_input() {
    SaveInput save;
    save.source_id = source_id_;
    save.blk = vm.user->BLK;
    save.tib = std::string(vm.tib_data, vm.tib_data + vm.user->NR_IN);
    save.tib_ptr = vm.tib_ptr;
    save.nr_in = vm.user->NR_IN;
    save.to_in = vm.user->TO_IN;

    input_stack_.push_back(save);
}

bool Input::restore_input() {
    if (input_stack_.empty()) {
        return false;
    }
    else {
        SaveInput save = input_stack_.back();
        input_stack_.pop_back();

        // close current file if any
        if (source_id_ > 0) {
            Error error_code = Error::None;
            vm.files->close(source_id_, error_code);
        }
        source_id_ = save.source_id;
        vm.user->BLK = save.blk;
        set_tib(save.tib);
        vm.tib_ptr = save.tib_ptr;
        vm.user->NR_IN = save.nr_in;
        vm.user->TO_IN = save.to_in;

        return true;
    }
}

void Input::save_input_for_query() {
    ++num_query_;
    save_input();
}

bool Input::restore_input_if_query() {
    if (num_query_ <= 0) {
        return false;
    }
    else {
        --num_query_;
        return restore_input();
    }
}

int Input::input_level() const {
    return static_cast<int>(input_stack_.size());
}

void Input::restore_input(int level) {
    while (level < input_level()) {
        restore_input();
    }
}

void f_source() {
    push(mem_addr(vm.input.buffer()));
    push(vm.user->NR_IN);
}

void f_tib() {
    push(mem_addr(vm.input.buffer()));
}

bool f_refill() {
    return vm.input.refill();
}

void f_accept() {
    uint max_size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, max_size);

    std::string line;
    if (std::getline(std::cin, line)) {
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();    // remove newline
        }

        uint size = static_cast<uint>(line.size());
        if (size > max_size) {
            size = max_size;
        }
        memcpy(buffer, line.c_str(), size);
        push(size);
    }
    else {
        push(0);
    }
}

void f_expect() {
    f_accept();
    vm.user->SPAN = pop();
}

void f_query() {
    vm.input.save_input_for_query();
    vm.input.open_terminal();
    vm.input.refill();
}

void f_save_input() {
    vm.input.save_input();
}

bool f_restore_input() {
    return !vm.input.restore_input(); // true if cannot be restored
}

