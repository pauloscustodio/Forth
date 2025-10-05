//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "parser.h"
#include "strings.h"
#include "vm.h"
#include <algorithm>
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
    if (vm.wordbuf_ptr + alloc_size > WORDBUF_SZ) {
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

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
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
        int str_addr = vm.dict.alloc_string(message, size);
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
        int str_addr = vm.dict.alloc_string(message, size);
        comma(xtXSLITERAL);
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
        int str_addr = vm.dict.alloc_string(message.c_str(),
                                            static_cast<uint>(message.size()));
        comma(xtXSLITERAL);
        comma(str_addr);
    }
    else {
        LongString* lstring = vm.wordbuf.append_long_string(message);
        push(mem_addr(lstring->str()));
        push(lstring->size());
    }
}

void f_xsliteral() {
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
        int str_addr = vm.dict.alloc_cstring(message);
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

void f_minus_trailing() {
    uint size = pop();
    uint addr = pop();
    const char* str = mem_char_ptr(addr, size);

    while (size > 0 && is_space(str[size - 1])) {
        size--;
    }

    addr = mem_addr(str);
    push(addr);
    push(size);
}

void f_slash_string() {
    int n = pop();
    uint size = pop();
    uint addr = pop();

    push(addr + n);
    push(size - n);
}

void f_blank() {
    int n = pop();
    uint addr = pop();
    if (n > 0) {
        memset(mem_char_ptr(addr, n), BL, n);
    }
}

void f_cmove() {
    int n = pop();
    uint dst_addr = pop();
    uint src_addr = pop();
    if (n > 0) {
        const char* src = mem_char_ptr(src_addr, n);
        char* dst = mem_char_ptr(dst_addr, n);
        // memcpy on ovelapped regions is undefined behaviour
        while (n-- > 0) {
            *dst++ = *src++;
        }
    }
}

void f_cmove_to() {
    int n = pop();
    uint dst_addr = pop();
    uint src_addr = pop();
    if (n > 0) {
        const char* src = mem_char_ptr(src_addr, n) + n;
        char* dst = mem_char_ptr(dst_addr, n) + n;
        // memcpy on ovelapped regions is undefined behaviour
        while (n-- > 0) {
            *--dst = *--src;
        }
    }
}

void f_compare() {
    uint len2 = pop();
    uint addr2 = pop();
    const char* str2 = mem_char_ptr(addr2, len2);

    uint len1 = pop();
    uint addr1 = pop();
    const char* str1 = mem_char_ptr(addr1, len1);

    if (len1 < len2) {
        int cmp = memcmp(str1, str2, len1);
        if (cmp == 0) {
            push(-1);
        }
        else if (cmp < 0) {
            push(-1);
        }
        else {
            push(1);
        }
    }
    else if (len1 > len2) {
        int cmp = memcmp(str1, str2, len2);
        if (cmp == 0) {
            push(1);
        }
        else if (cmp < 0) {
            push(-1);
        }
        else {
            push(1);
        }
    }
    else {
        int cmp = memcmp(str1, str2, len1);
        if (cmp == 0) {
            push(0);
        }
        else if (cmp < 0) {
            push(-1);
        }
        else {
            push(1);
        }
    }
}

void f_search() {
    uint len2 = pop();
    uint addr2 = pop();
    const char* str2 = mem_char_ptr(addr2, len2);
    uint len1 = pop();
    uint addr1 = pop();
    const char* str1 = mem_char_ptr(addr1, len1);

    if (len2 == 0) {
        // empty string always matches at start
        push(addr1);          // address of match
        push(len1);          // length of match to end
        push(F_TRUE);
    }
    else if (len1 == 0 || len2 > len1) {
        // empty string or str2 longer than str1 never matches
        push(addr1);
        push(len1);
        push(F_FALSE);
    }
    else {
        const char* p = std::search(str1, str1 + len1, str2, str2 + len2);
        if (p != str1 + len1) {
            push(mem_addr(p));                      // address of match
            push(static_cast<int>(str1 + len1 - p));// length of match to end
            push(F_TRUE);
        }
        else {
            push(addr1);
            push(len1);
            push(F_FALSE);
        }
    }
}

void f_sliteral() {
    uint size = pop();
    uint addr = pop();
    const char* str = mem_char_ptr(addr, size);

    // save copy of the string in names space
    uint saved_addr = vm.dict.alloc_string(str, size);

    // save execution code
    comma(xtXSLITERAL);
    comma(saved_addr);
}

void f_replaces() {
    uint name_len = pop();
    uint name_addr = pop();
    const char* name_str = mem_char_ptr(name_addr, name_len);
    std::string name = std::string(name_str, name_str + name_len);

    uint text_len = pop();
    uint text_addr = pop();
    const char* text_str = mem_char_ptr(text_addr, text_len);
    std::string text = std::string(text_str, text_str + text_len);

    vm.substitutions[to_upper(name)] = text;
}

static std::string substitute(const std::string& input,
                              const std::unordered_map<std::string, std::string>& substitutions,
                              int& count) {
    std::string result;
    size_t pos = 0;
    count = 0;

    while (pos < input.size()) {
        size_t start = input.find('%', pos);
        if (start == std::string::npos) {
            result += input.substr(pos);
            break;
        }

        size_t end = input.find('%', start + 1);
        if (end == std::string::npos) {
            result += input.substr(pos);
            break;
        }

        result += input.substr(pos, start - pos);  // text before %NAME%
        std::string key = input.substr(start + 1, end - start - 1);

        if (key.empty()) {
            result += "%";  // handle %% as literal %
            pos = end + 1;
            continue;
        }

        auto it = substitutions.find(to_upper(key));
        if (it != substitutions.end()) {
            result += it->second;  // replace with value
            ++count;
        }
        else {
            result += "%" + key + "%";  // leave unchanged
        }

        pos = end + 1;
    }

    return result;
}

static std::string escape_percents(const std::string& input) {
    std::string result;
    result.reserve(input.size() *
                   2);  // Reserve enough space to avoid reallocations

    for (char ch : input) {
        if (ch == '%') {
            result += "%%";
        }
        else {
            result += ch;
        }
    }

    return result;
}

void f_substitute() {
    uint buffer_len = pop();
    uint buffer_addr = pop();
    char* buffer_str = mem_char_ptr(buffer_addr, buffer_len);

    uint str_len = pop();
    uint str_addr = pop();
    const char* str_str = mem_char_ptr(str_addr, str_len);
    std::string str = std::string(str_str, str_str + str_len);

    int count = 0;
    std::string result = substitute(str, vm.substitutions, count);
    if (static_cast<uint>(result.size()) > buffer_len) {
        push(0);
        push(0);
        push(static_cast<int>(Error::SubstituteException));
    }
    else {
        memcpy(buffer_str, result.c_str(), result.size());
        push(buffer_addr);
        push(static_cast<int>(result.size()));
        push(count);
    }
}

void f_unescape() {
    uint buffer_addr = pop();
    char* buffer_str = mem_char_ptr(buffer_addr);

    uint str_len = pop();
    uint str_addr = pop();
    const char* str_str = mem_char_ptr(str_addr, str_len);
    std::string str = std::string(str_str, str_str + str_len);

    std::string result = escape_percents(str);
    memcpy(buffer_str, result.c_str(), result.size());
    push(buffer_addr);
    push(static_cast<int>(result.size()));
}

