//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "parser.h"
#include "str.h"
#include "vm.h"
#include <cstring>

uint CString::size() const {
    return size_;
}

const char* CString::str() const {
    return str_;
}

std::string CString::to_string() const {
    return std::string(str_, str_ + size_);
}

uint CString::alloc_size(uint num_chars) {
    if (num_chars > MAX_CSTRING_SZ) {
        error(Error::ParsedStringOverflow, std::to_string(num_chars));
    }

    return aligned(1 + num_chars + 1); // count + chars + BL after string
}

void CString::set_cstring(const char* str, uint size) {
    if (size > MAX_CSTRING_SZ) {
        error(Error::ParsedStringOverflow, std::string(str, str + size));
    }

    size_ = static_cast<uchar>(size);
    memcpy(str_, str, size);
    str_[size] = BL; // BL after the string
}

void CString::set_cstring(const std::string& str) {
    set_cstring(str.c_str(), static_cast<uint>(str.size()));
}

std::string LongString::to_string() const {
    return std::string(str_, str_ + size_);
}

uint LongString::alloc_size(uint num_chars) {
    if (num_chars > BUFFER_SZ) {
        error(Error::InputBufferOverflow, std::to_string(num_chars));
    }

    return aligned(sizeof(size_) + num_chars +
                   1); // count + chars + BL after string
}

void LongString::set_string(const char* str, uint size) {
    if (size > BUFFER_SZ) {
        error(Error::InputBufferOverflow, std::string(str, str + size));
    }

    size_ = size;
    memcpy(str_, str, size);
    str_[size] = BL; // BL after the string
}

void LongString::set_string(const std::string& str) {
    set_string(str.c_str(), static_cast<uint>(str.size()));
}

void Wordbuf::init() {
    memset(vm.wordbuf_data, BL, WORDBUF_SZ);
    vm.wordbuf_ptr = 0;
}

CString* Wordbuf::append_cstring(const std::string& str) {
    return append_cstring(str.c_str(), static_cast<uint>(str.size()));
}

CString* Wordbuf::append_cstring(const char* str, uint size) {
    if (size > MAX_CSTRING_SZ) {
        error(Error::ParsedStringOverflow, std::string(str, str + size));
    }

    uint alloc_size = CString::alloc_size(size);
    if (vm.wordbuf_ptr + alloc_size > sizeof(vm.wordbuf_data)) {
        vm.wordbuf_ptr = 0;
    }
    CString* cstring = reinterpret_cast<CString*>(vm.wordbuf_data + vm.wordbuf_ptr);
    vm.wordbuf_ptr += alloc_size;

    cstring->set_cstring(str, size);

    return cstring;
}

LongString* Wordbuf::append_long_string(const std::string& str) {
    return append_long_string(str.c_str(), static_cast<uint>(str.size()));
}

LongString* Wordbuf::append_long_string(const char* str, uint size) {
    if (size > BUFFER_SZ) {
        error(Error::InputBufferOverflow, std::string(str, str + size));
    }

    uint alloc_size = LongString::alloc_size(size);
    if (vm.wordbuf_ptr + alloc_size > WORDBUF_SZ) {
        vm.wordbuf_ptr = 0;
    }
    LongString* lstring = reinterpret_cast<LongString*>(
                              vm.wordbuf_data + vm.wordbuf_ptr);
    vm.wordbuf_ptr += alloc_size;

    lstring->set_string(str, size);

    return lstring;
}

static char to_lower(char c) {
    return tolower(static_cast<unsigned char>(c));
}

bool case_insensitive_equal(const std::string& a, const std::string& b) {
    return case_insensitive_equal(a.c_str(), static_cast<uint>(a.size()),
                                  b.c_str(), static_cast<uint>(b.size()));
}

bool case_insensitive_equal(const char* a_str, uint a_size, const char* b_str,
                            uint b_size) {
    if (a_size != b_size) {
        return false;
    }
    for (uint i = 0; i < a_size; ++i) {
        if (to_lower(a_str[i]) != to_lower(b_str[i])) {
            return false;
        }
    }
    return true;
}

void f_count() {
    uint addr = pop();
    int len = cfetch(addr++);
    push(addr);
    push(len);
}

void f_dot_quote() {
    uint size;
    const char* message = parse_word(size, '"');
    if (vm.user->STATE == STATE_COMPILE) {
        int str_addr = vm.dict->alloc_string(message, size);
        comma(xtXDOT_QUOTE);
        comma(str_addr);
    }
    else {
        print_string(message, size);
    }
}

void f_xdot_quote() {
    int str_addr = fetch(vm.ip);
    vm.ip += CELL_SZ;
    const LongString* message = reinterpret_cast<const LongString*>(mem_char_ptr(
                                    str_addr));
    print_string(message->str(), message->size());
}

void f_s_quote() {
    uint size;
    const char* message = parse_word(size, '"');
    if (vm.user->STATE == STATE_COMPILE) {
        int str_addr = vm.dict->alloc_string(message, size);
        comma(xtXS_QUOTE);
        comma(str_addr);
    }
    else {
        LongString* lstring = vm.wordbuf.append_long_string(message, size);
        push(mem_addr(lstring->str()));
        push(lstring->size());
    }
}

void f_s_backslash_quote() {
    std::string message = parse_backslash_string();
    if (vm.user->STATE == STATE_COMPILE) {
        int str_addr = vm.dict->alloc_string(message.c_str(),
                                             static_cast<uint>(message.size()));
        comma(xtXS_QUOTE);
        comma(str_addr);
    }
    else {
        LongString* lstring = vm.wordbuf.append_long_string(message);
        push(mem_addr(lstring->str()));
        push(lstring->size());
    }
}

void f_xs_quote() {
    int str_addr = fetch(vm.ip);
    vm.ip += CELL_SZ;
    const LongString* message = reinterpret_cast<const LongString*>(mem_char_ptr(
                                    str_addr));
    push(mem_addr(message->str()));
    push(message->size());
}

void f_c_quote() {
    const CString* message = parse_cword('"');
    if (vm.user->STATE == STATE_COMPILE) {
        int str_addr = vm.dict->alloc_cstring(message);
        comma(xtXC_QUOTE);
        comma(str_addr);
    }
    else {
        push(mem_addr(message));
    }
}

void f_xc_quote() {
    int str_addr = fetch(vm.ip);
    vm.ip += CELL_SZ;
    const CString* message = reinterpret_cast<const CString*>(mem_char_ptr(
                                 str_addr));
    push(mem_addr(message));
}

void f_dot_paren() {
    uint size;
    const char* message = parse_word(size, ')');
    print_string(message, size);
}

void f_slash_string() {
    int n = pop();
    uint size = pop();
    uint addr = pop();

    push(addr + n);
    push(size - n);
}

