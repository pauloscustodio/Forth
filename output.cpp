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
using namespace std;

void NumberOutput::init() {
    start();
}

void NumberOutput::start() {
    m_ptr = PAD_SZ;
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

void NumberOutput::end(void) {
    dpop();     // drop number
    push(vm.mem.addr(m_buffer + m_ptr));
    push(PAD_SZ - m_ptr);
}

void fTYPE() {
    int len = pop();
    int addr = pop();
    const char* str = vm.mem.char_ptr(addr);
    for (int i = 0; i < len; ++i)
        cout << str[i];
}

void fEMIT() {
    char c = static_cast<char>(pop());
    cout << c;
}

void fCR() {
    cout << endl;
}

void fSPACE() {
    cout << BL;
}

void fSPACES() {
    int size = pop();
    for (int i = 0; i < size; ++i)
        cout << BL;
}

void fLESS_HASH() {
    vm.number_output->start();
}

void fHASH() {
    vm.number_output->add_digit();
}

void fHASH_S() {
    vm.number_output->add_digits();
}

void fHOLD() {
    char c = static_cast<char>(pop());
    vm.number_output->add_char(c);
}

void fSIGN() {
    int sign = pop();
    vm.number_output->add_sign(sign);
}

void fHASH_GREATER() {
    vm.number_output->end();
}

static void print_dint_uint(int sign) {
    fLESS_HASH();
    push(BL); fHOLD();
    fHASH_S();
    push(sign); fSIGN();
    fHASH_GREATER();
    fTYPE();
}

static void print_dint_uint_aligned(int width, int sign) {
    fLESS_HASH();

    dint d;
    do {
        fHASH();
        width--;
        d = dpeek();
    } while (d != 0);

    if (sign < 0) {
        push(sign); fSIGN();
        width--;
    }

    while (width-- > 0) {
        push(BL); fHOLD();
    }

    fHASH_GREATER();
    fTYPE();
}

void cDOT(int value) {
    dpush(cDABS(value));
    int sign = value;
    print_dint_uint(sign);
}

void fDOT() {
    int value = pop();
    cDOT(value);
}

void cD_DOT(dint value) {
    dpush(cDABS(value));
    print_dint_uint(value < 0 ? -1 : 1);
}

void fD_DOT() {
    dint value = dpop();
    cD_DOT(value);
}

void cD_DOT_R(dint value, int width) {
    dpush(cDABS(value));
    print_dint_uint_aligned(width, value < 0 ? -1 : 1);
}

void fD_DOT_R() {
    int width = pop();
    dint value = dpop();
    cD_DOT_R(value, width);
}

void cU_DOT(uint value) {
    push(value);    // lo
    push(0);        // hi
    print_dint_uint(+1);
}

void fU_DOT() {
    uint value = static_cast<uint>(pop());
    cU_DOT(value);
}

void cDOT_R(int value, int width) {
    dpush(cDABS(value));
    print_dint_uint_aligned(width, value);
}

void fDOT_R() {
    int width = pop();
    int value = pop();
    cDOT_R(value, width);
}

void cU_DOT_R(uint value, int width) {
    push(value);
    push(0);
    print_dint_uint_aligned(width, 1);
}

void fU_DOT_R() {
    int width = pop();
    uint value = static_cast<uint>(pop());
    cU_DOT_R(value, width);
}
