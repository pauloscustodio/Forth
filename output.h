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
    void add_string(const string& str);
    void add_string(const char* str, size_t size);
    void add_string(const char* str, int size);
    void end() const;
    void end_print() const;

private:
    char buffer_[PAD_SZ];
    int ptr_;
};

string char_to_string(char c);
void print_char(char c);
void print_string(const string& str);

string spaces_to_string(int size);
void print_spaces(int size);

string string_to_string(int addr, int size);
string string_to_string(const char* str, int size);

void print_string(int addr, int size);
void print_string(const char* str, int size);

string number_to_string(int value);
void print_number(int value);

string number_to_string(dint value);
void print_number(dint value);

string number_dot_to_string(dint value);
void print_number_dot(dint value);

string number_to_string(int value, int width);
void print_number(int value, int width);

string number_to_string(dint value, int width);
void print_number(dint value, int width);

string unsigned_number_to_string(uint value);
void print_unsigned_number(uint value);

string unsigned_number_to_string(uint value, int width);
void print_unsigned_number(uint value, int width);
