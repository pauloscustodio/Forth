//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

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
    void end(void);

private:
    char m_buffer[PAD_SZ];
    int m_ptr;
};

void cDOT(int value);
void cD_DOT(dint value);
void cD_DOT_R(dint value, int width);
void cU_DOT(uint value);
void cDOT_R(int value, int width);
void cU_DOT_R(uint value, int width);
