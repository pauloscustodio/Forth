//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>

class CString {
public:
    uint size() const {
        return size_;
    }
    const char* str() const {
        return str_;
    }

    std::string to_string() const;
    static int alloc_size(int num_chars);

    // user must allocate alloc_size() bytes
    void set_cstring(const char* str, uint size);
    void set_cstring(const std::string& str);

private:
    uchar size_;     // size of string
    char str_[1];    // flexible array member
};


class LongString {
public:
    uint size() const {
        return size_;
    }
    const char* str() const {
        return str_;
    }

    std::string to_string() const;
    static int alloc_size(int num_chars);

    // user must allocate alloc_size() bytes
    void set_string(const char* str, uint size);
    void set_string(const std::string& str);

private:
    uint size_;      // size of string
    char str_[1];   // flexible array member
};


class Wordbuf {
public:
    void init();

    CString* append_cstring(const std::string& str);
    CString* append_cstring(const char* str, uint size);

    LongString* append_long_string(const std::string& str);
    LongString* append_long_string(const char* str, uint size);

private:
    char data_[WORDBUF_SZ];
    int ptr_;
};


bool case_insensitive_equal(const std::string& a, const std::string& b);
bool case_insensitive_equal(
    const char* a_str, uint a_size,
    const char* b_str, uint b_size);

void f_count();
void f_dot_quote();
void f_xdot_quote();
void f_s_quote();
void f_s_backslash_quote();
void f_xs_quote();
void f_c_quote();
void f_xc_quote();
void f_dot_paren();
void f_slash_string();
