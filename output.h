//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include "forth.h"
using namespace std;

class NumberOutput {
public:
    void init();

    void start();
    void add_digit();
    void add_digits();
    void add_char(char c);
    void add_sign(int sign);
    void end() const;
    void end_print() const;

private:
    char buffer_[PAD_SZ];
    int ptr_;
};

void print_char(char c);
void print_spaces(int size);
void print_string(int addr, int size);
void print_string(const char* str, int size);

void print_number(int value);
void print_number(dint value);
void print_number(int value, int width);
void print_number(dint value, int width);
void print_unsigned_number(uint value);
void print_unsigned_number(uint value, int width);
