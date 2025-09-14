//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "output.h"
#include "stack.h"
#include <cstring>
#include <iostream>

void Stack::init(char prefix, Error err_underflow, Error err_overflow) {
    memset(data_, 0, sizeof(data_));
    sp_ = STACK_SZ;
    prefix_ = prefix;
    err_underflow_ = err_underflow;
    err_overflow_ = err_overflow;
}

void Stack::push(int value) {
    if (sp_ <= 0) {
        error(err_overflow_);
    }
    else {
        data_[--sp_] = value;
    }
}

int Stack::pop() {
    if (sp_ >= STACK_SZ) {
        error(err_underflow_);
        return 0;
    }
    else {
        return data_[sp_++];
    }
}

int Stack::peek(int depth) const {
    int idx = sp_ + depth;
    if (idx >= STACK_SZ) {
        error(err_underflow_);
        return 0;
    }
    else {
        return data_[idx];
    }
}

void Stack::dpush(dint value) {
    push(dcell_lo(value));
    push(dcell_hi(value));
}

dint Stack::dpop() {
    int hi = pop();
    int lo = pop();
    return mk_dcell(hi, lo);
}

dint Stack::dpeek(int depth) const {
    int hi = peek(2 * depth);
    int lo = peek(2 * depth + 1);
    return mk_dcell(hi, lo);
}

// e.g. roll(1)
// | 1 | 2 |    --> | 2 | 1 |
// ^sp ^bot
void Stack::roll(int depth) {
    if (depth == 0)
        ; // ignore
    else {
        int bot = sp_ + depth;
        if (bot >= STACK_SZ) {
            error(err_underflow_);
        }
        else {
            int bot_value = data_[bot];
            memmove(&data_[sp_ + 1], &data_[sp_], depth * CELL_SZ);
            data_[sp_] = bot_value;
        }
    }
}

void Stack::print() const {
    std::cout << "(";
    if (prefix_ != '\0') {
        std::cout << prefix_ << ":";
    }
    std::cout << " ";
    for (int i = STACK_SZ - 1; i >= sp_; i--) {
        print_number(data_[i]);
    }
    std::cout << ") ";
}

// so that we can debug <# ... #>
void Stack::print_debug() const {
    std::cout << "(";
    if (prefix_ != '\0') {
        std::cout << prefix_ << ":";
    }
    std::cout << " ";
    for (int i = STACK_SZ - 1; i >= sp_; i--) {
        std::cout << data_[i] << BL;
    }
    std::cout << ") ";
}

void Stack::set_sp(int sp) {
    if (sp < 0 || sp > STACK_SZ) {
        error(Error::InvalidMemoryAddress);
    }
    sp_ = sp;
}

void Stack::clear() {
    sp_ = STACK_SZ;
}

