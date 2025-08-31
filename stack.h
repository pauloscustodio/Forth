//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include "forth.h"
#include <string>
using namespace std;

class Stack {
public:
    void init(char prefix, Error err_underflow, Error err_overflow);

    void push(int value);
    int pop();
    int peek(int depth = 0) const;
    void dpush(dint value);
    dint dpop();
    dint dpeek(int depth = 0) const;
    int depth() const {
        return STACK_SZ - sp_;
    }
    void roll(int depth);

    int sp() const {
        return sp_;
    }
    void set_sp(int sp);
    void clear();

    void print() const;
    void print_debug() const;

private:
    int data_[STACK_SZ];
    int sp_;
    char prefix_;
    Error err_underflow_;
    Error err_overflow_;
};

