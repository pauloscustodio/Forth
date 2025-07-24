//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "math.h"
#include "output.h"
#include "vm.h"
#include <cstring>
using namespace std;

void NumberOutput::init() {
    memset(m_buffer, BL, sizeof(m_buffer));
    start();
}

void NumberOutput::start() {
    m_ptr = static_cast<int>(sizeof(m_buffer));
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
    if (m_ptr < 1)
        error(Error::NumberOutputOverflow);
    else
        m_buffer[--m_ptr] = c;
}

void NumberOutput::add_sign(int sign) {
    if (sign < 0)
        add_char('-');
}

void NumberOutput::end() const {
    dpop();     // drop number
    push(mem_addr(m_buffer + m_ptr));
    push(static_cast<int>(sizeof(m_buffer)) - m_ptr);
}

void NumberOutput::end_print() const {
    dpop();     // drop number
    const char* str = m_buffer + m_ptr;
    int size = static_cast<int>(sizeof(m_buffer)) - m_ptr;
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
        fHASH();
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

void print_string(int addr, int size) {
    print_string(mem_char_ptr(addr), size);
}

void print_string(const char* str, int size) {
    for (int i = 0; i < size; ++i)
        cout << str[i];
}

void print_number(int value) {
    dpush(dabs(value));
    int sign = value;
    print_dint_uint(sign);
}

void print_number(dint value) {
    dpush(dabs(value));
    print_dint_uint(value < 0 ? -1 : 1);
}

void print_number(dint value, int width) {
    dpush(dabs(value));
    print_dint_uint_aligned(width, value < 0 ? -1 : 1);
}

void print_unsigned_number(uint value) {
    push(value);    // lo
    push(0);        // hi
    print_dint_uint(+1);
}

void print_number(int value, int width) {
    dpush(dabs(value));
    print_dint_uint_aligned(width, value);
}

void print_unsigned_number(uint value, int width) {
    push(value);
    push(0);
    print_dint_uint_aligned(width, 1);
}

