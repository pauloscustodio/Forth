//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "math.h"
#include "output.h"
#include "vm.h"
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;

void NumberOutput::init() {
    memset(buffer_, BL, sizeof(buffer_));
    start();
}

void NumberOutput::start() {
    ptr_ = static_cast<int>(sizeof(buffer_));
}

void NumberOutput::add_digit() {
    // compute digit
    udint value = dpop();

    // can use % instead of f_mod because value is assumed positive
    int digit = static_cast<int>(value % vm.user->BASE);

    value /= vm.user->BASE;
    dpush(value);

    // output it
    if (digit < 10)
        add_char('0' + digit);
    else
        add_char('A' + digit - 10);
}

void NumberOutput::add_digits() {
    dint value;
    do {
        add_digit();
        value = dpeek();
    } while (value != 0);
}

void NumberOutput::add_char(char c) {
    if (ptr_ < 1)
        error(Error::PicturedNumericOutputStringOverflow);
    else
        buffer_[--ptr_] = c;
}

void NumberOutput::add_sign(int sign) {
    if (sign < 0)
        add_char('-');
}

void NumberOutput::add_string(const string& str) {
    add_string(str.c_str(), str.size());
}

void NumberOutput::add_string(const char* str, size_t size) {
    add_string(str, static_cast<int>(size));
}

void NumberOutput::add_string(const char* str, int size) {
    for (int i = size - 1; i >= 0; --i)
        add_char(str[i]);
}

void NumberOutput::end() const {
    dpop();     // drop number
    push(mem_addr(buffer_ + ptr_));
    push(static_cast<int>(sizeof(buffer_)) - ptr_);
}

void NumberOutput::end_print() const {
    dpop();     // drop number
    const char* str = buffer_ + ptr_;
    int size = static_cast<int>(sizeof(buffer_)) - ptr_;
    print_string(str, size);
}

static string print_dint_uint(int sign) {
    vm.number_output->start();
    vm.number_output->add_char(BL);
    vm.number_output->add_digits();
    vm.number_output->add_sign(sign);
    vm.number_output->end();
    int size = pop();
    int addr = pop();
    char* str = mem_char_ptr(addr);
    return string(str, str + size);
}

static string print_dint_uint_aligned(int width, int sign) {
    vm.number_output->start();

    dint d;
    do {
        vm.number_output->add_digit();
        width--;
        d = dpeek();
    } while (d != 0);

    if (sign < 0) {
        vm.number_output->add_sign(sign);
        width--;
    }

    while (width-- > 0) {
        vm.number_output->add_char(BL);
    }

    vm.number_output->end();
    int size = pop();
    int addr = pop();
    char* str = mem_char_ptr(addr);
    return string(str, str + size);
}

string char_to_string(char c) {
    ostringstream oss;
    oss << c;
    return oss.str(); 
}

void print_char(char c) {
    cout << char_to_string(c);
}

void print_string(const string& str) {
    for (char c : str) {
        print_char(c);
    }
}

string spaces_to_string(int size) {
    if (size < 1)
        return "";
    else
        return string(size, BL);
}

void print_spaces(int size) {
    print_string(spaces_to_string(size));
}

string string_to_string(int addr, int size) {
    return string_to_string(mem_char_ptr(addr), size);
}

string string_to_string(const char* str, int size) {
    ostringstream oss;
    for (int i = 0; i < size; ++i)
        oss << str[i];
    return oss.str();
}

void print_string(int addr, int size) {
    print_string(mem_char_ptr(addr), size);
}

void print_string(const char* str, int size) {
    print_string(string_to_string(str, size));
}

string number_to_string(int value) {
    dpush(f_dabs(value));
    int sign = value;
    return print_dint_uint(sign);
}

void print_number(int value) {
    print_string(number_to_string(value));
}

string number_to_string(dint value) {
    dpush(f_dabs(value));
    int sign = value < 0 ? -1 : 1;
    return print_dint_uint(sign);
}

void print_number(dint value) {
    print_string(number_to_string(value));
}

string number_dot_to_string(dint value) {
    string number = number_to_string(value, 0);
    number.push_back('.');
    number.push_back(BL);
    return number;
}

void print_number_dot(dint value) {
    print_string(number_dot_to_string(value));
}

string number_to_string(int value, int width) {
    dpush(f_dabs(value));
    return print_dint_uint_aligned(width, value);
}

void print_number(int value, int width) {
    print_string(number_to_string(value, width));
}

string number_to_string(dint value, int width) {
    dpush(f_dabs(value));
    int sign = value < 0 ? -1 : 1;
    return print_dint_uint_aligned(width, sign);
}

void print_number(dint value, int width) {
    print_string(number_to_string(value, width));
}

string unsigned_number_to_string(uint value) {
    push(value);    // lo
    push(0);        // hi
    return print_dint_uint(+1);
}

void print_unsigned_number(uint value) {
    print_string(unsigned_number_to_string(value));
}

string unsigned_number_to_string(uint value, int width) {
    push(value);
    push(0);
    return print_dint_uint_aligned(width, 1);
}

void print_unsigned_number(uint value, int width) {
    print_string(unsigned_number_to_string(value, width));
}
