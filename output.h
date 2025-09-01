//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include "forth.h"

class NumberOutput {
public:
    void init();

    void start();
    void add_digit();
    void add_digits();
    void add_char(char c);
    void add_sign(int sign);
    void add_string(const std::string& str);
    void add_string(const char* str, uint size);
    void end() const;
    void end_print() const;

private:
    char buffer_[PAD_SZ];
    int ptr_;
};

std::string char_to_string(char c);
void print_char(char c);
void print_string(const std::string& str);

std::string spaces_to_string(int count);
void print_spaces(int count);

std::string string_to_string(uint addr, uint size);
std::string string_to_string(const char* str, uint size);

void print_string(uint addr, uint size);
void print_string(const char* str, uint size);

std::string number_to_string(int value);
void print_number(int value);

std::string number_to_string(dint value);
void print_number(dint value);

std::string number_dot_to_string(dint value);
void print_number_dot(dint value);

std::string number_to_string(int value, int width);
void print_number(int value, int width);

std::string number_to_string(dint value, int width);
void print_number(dint value, int width);

std::string unsigned_number_to_string(uint value);
void print_unsigned_number(uint value);

std::string unsigned_number_to_string(uint value, int width);
void print_unsigned_number(uint value, int width);
