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
    dint value = dpop();

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
        error(Error::NumberOutputOverflow);
    else
        buffer_[--ptr_] = c;
}

void NumberOutput::add_sign(int sign) {
    if (sign < 0)
        add_char('-');
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

static void print_dint_uint(int sign) {
    vm.number_output->start();
    vm.number_output->add_char(BL);
    vm.number_output->add_digits();
    vm.number_output->add_sign(sign);
    vm.number_output->end_print();
}

static void print_dint_uint_aligned(int width, int sign) {
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

    vm.number_output->end_print();
}

void print_char(char c) {
    cout << c;
}

void print_spaces(int size) {
    for (int i = 0; i < size; ++i)
        print_char(BL);
}

void print_string(int addr, int size) {
    print_string(mem_char_ptr(addr), size);
}

void print_string(const char* str, int size) {
    for (int i = 0; i < size; ++i)
        print_char(str[i]);
}

void print_number(int value) {
    dpush(f_dabs(value));
    int sign = value;
    print_dint_uint(sign);
}

void print_number(dint value) {
    dpush(f_dabs(value));
    print_dint_uint(value < 0 ? -1 : 1);
}

void print_number(dint value, int width) {
    dpush(f_dabs(value));
    print_dint_uint_aligned(width, value < 0 ? -1 : 1);
}

void print_unsigned_number(uint value) {
    push(value);    // lo
    push(0);        // hi
    print_dint_uint(+1);
}

void print_number(int value, int width) {
    dpush(f_dabs(value));
    print_dint_uint_aligned(width, value);
}

void print_unsigned_number(uint value, int width) {
    push(value);
    push(0);
    print_dint_uint_aligned(width, 1);
}
