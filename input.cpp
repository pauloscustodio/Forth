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
using namespace std;

void Pad::init() {
    memset(m_pad, BL, sizeof(m_pad));
}

void Input::init() {
    filename_ = new string;
    input_file_ = new ifstream();
    source_id_ = 0;
    buffer_ = tib_;
	memset(tib_, BL, sizeof(tib_));
    input_stack_ = new vector<SaveInput>;
    num_query_ = 0;
}

void Input::deinit() {
    delete filename_;
    delete input_file_;
    delete input_stack_;
}

void Input::open_file(const char* filename, int size) {
    open_file(string(filename, filename + size));
}

void Input::open_file(const char* filename, size_t size) {
    open_file(string(filename, filename + size));
}

void Input::open_file(const string& filename) {
    *filename_ = filename;

    if (input_file_->is_open()) 
        input_file_->close();
    
    input_file_->open(filename, ios::binary);
    if (!input_file_->is_open()) 
        error(Error::OpenFileException, filename);

    source_id_ = 1; // file
    set_tib("", 0);
}

void Input::open_terminal() {
    if (input_file_->is_open())
        input_file_->close();

    source_id_ = 0; // terminal
    set_tib("", 0);
}

void Input::set_text(const char* text, int size) {
    if (input_file_->is_open())
        input_file_->close();

    source_id_ = -1; // string
    vm.user->NR_IN = size;
    vm.user->TO_IN = 0;
    buffer_ = text;
}

void Input::set_text(const char* text, size_t size) {
    set_text(text, static_cast<int>(size));
}

void Input::set_block(Block* block) {
    vm.user->BLK = block->blk;
    vm.user->NR_IN = BLOCK_SZ;
    vm.user->TO_IN = 0;
    buffer_ = block->block;
    source_id_ = 0;
}

void Input::set_tib(const char* text, int size) {
    if (size > BUFFER_SZ)
        error(Error::InputBufferOverflow);

    vm.user->NR_IN = size;
    vm.user->TO_IN = 0;
    memcpy(tib_, text, size);
    tib_[size] = BL; // BL after the string
    buffer_ = tib_;
}

void Input::set_tib(const char* text, size_t size) {
    set_tib(text, static_cast<int>(size));
}

void Input::set_tib(const string& text) {
    set_tib(text.c_str(), text.size());
}

bool Input::refill() {
    bool ok = false;
    string line;

    if (source_id_ < 0) {               // input from string
        return false;       
    }
    else if (vm.user->BLK > 0) {        // input from block
        if (vm.user->BLK + 1 >= vm.blocks->num_blocks())
            return false;
        else {
            ++vm.user->BLK;
            Block* block = vm.blocks->f_block(vm.user->BLK);
            set_block(block);

            if (vm.user->TRACE) {
                cout << endl << "> ";
                print_string(block->block, BLOCK_SZ);
                cout << endl;
            }

            return true;
        }
    }
    else if (source_id_ == 0) {         // input from terminal
        ok = static_cast<bool>(std::getline(cin, line)); 
    }
    else {                              // input from file
        ok = static_cast<bool>(std::getline(*input_file_, line)); 
    }

    set_tib(line);
    vm.user->NR_IN = static_cast<int>(line.size());
    vm.user->TO_IN = 0;

    if (ok && vm.user->TRACE) {
        cout << endl << "> " << line << endl;
    }

    return ok;
}

// save input context
void Input::save_input() {
    SaveInput save;
    save.filename = *filename_;
    if (input_file_->is_open()) {
        save.is_open = true;
        save.fpos = input_file_->tellg();
        input_file_->close();
    }
    else {
        save.is_open = false;
        save.fpos = 0;
    }

    save.source_id = source_id_;
    save.blk = vm.user->BLK;
    save.tib = string(tib_, tib_ + vm.user->NR_IN);
    save.buffer = buffer_;
    save.nr_in = vm.user->NR_IN;
    save.to_in = vm.user->TO_IN;

    input_stack_->push_back(save);
}

bool Input::restore_input() {
    if (input_stack_->empty())
        return false;
    else {
        if (input_file_->is_open())
            input_file_->close();
        
        SaveInput save = input_stack_->back();
        input_stack_->pop_back();

        *filename_ = save.filename;
        if (save.is_open) {
            input_file_->open(*filename_, ios::binary);
            if (!input_file_->is_open())
                error(Error::OpenFileException, *filename_);

            input_file_->seekg(save.fpos);
        }
        source_id_ = save.source_id;
        vm.user->BLK = save.blk;
        set_tib(save.tib);
        buffer_ = save.buffer;
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
    if (num_query_ <= 0)
        return false;
    else {
        --num_query_;
        return restore_input();
    }
}

int Input::input_level() const {
    return static_cast<int>(input_stack_->size());
}

void Input::restore_input(int level) {
    while (level > input_level())
        restore_input();
}

void f_source() {
    push(mem_addr(vm.input->buffer()));
    push(vm.user->NR_IN);
}

void f_tib() {
    push(mem_addr(vm.input->buffer()));
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

void f_expect() {
    f_accept();
    vm.user->SPAN = pop();
}

void f_query() {
    vm.input->save_input_for_query();
    vm.input->open_terminal();
    vm.input->refill();
}

void f_save_input() {
    vm.input->save_input();
}

bool f_restore_input() {
    return !vm.input->restore_input(); // true if cannot be restored
}

