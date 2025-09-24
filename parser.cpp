//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "parser.h"
#include "vm.h"

// ignore all control characters as spaces
bool is_space(char c) {
    return c >= 0 && c <= BL;
}

bool is_print(char c) {
    return c >= BL && c < 0x7f;
}

void strip_blanks(const char*& str, uint& size) {
    while (size > 0 && is_space(str[0])) {
        str++;
        size--;
    }
    while (size > 0 && is_space(str[size - 1])) {
        size--;
    }
}

// return digit value of character, or -1 if not a digit
static int char_digit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    else {
        return -1;    // not a digit
    }
}

static void skip_blanks() {
    const char* buffer = vm.input.buffer();

    while (vm.user->TO_IN < vm.user->NR_IN  && is_space(buffer[vm.user->TO_IN])) {
        ++vm.user->TO_IN;
    }
}

static int skip_to_delimiter(char delimiter, bool& found) {
    found = false;
    const char* buffer = vm.input.buffer();

    int end = vm.user->TO_IN;
    if (delimiter == BL) {
        while (vm.user->TO_IN < vm.user->NR_IN && !is_space(buffer[vm.user->TO_IN])) {
            ++vm.user->TO_IN;
        }

        end = vm.user->TO_IN;

        if (vm.user->TO_IN < vm.user->NR_IN && is_space(buffer[vm.user->TO_IN])) {
            found = true;
            ++vm.user->TO_IN;    // skip delimiter
        }
    }
    else {
        while (vm.user->TO_IN < vm.user->NR_IN && buffer[vm.user->TO_IN] != delimiter) {
            ++vm.user->TO_IN;
        }

        end = vm.user->TO_IN;

        if (vm.user->TO_IN < vm.user->NR_IN && buffer[vm.user->TO_IN] == delimiter) {
            found = true;
            ++vm.user->TO_IN;    // skip delimiter
        }

    }

    return end;	// end of word, char before delimiter
}

const char* parse_word(uint& size, char delimiter) {
    if (delimiter == BL) {
        skip_blanks();    // skip blanks before word
    }

    const char* buffer = vm.input.buffer();
    uint start = vm.user->TO_IN;
    bool found;
    uint end = skip_to_delimiter(delimiter, found);

    size = end - start;
    const char* word = &buffer[start];
    return word;
}

CString* parse_cword(char delimiter) {
    uint size = 0;
    const char* word = parse_word(size, delimiter);
    CString* cword = vm.wordbuf.append_cstring(word, size);
    return cword;
}

std::string parse_backslash_string() {
    const char* buffer = vm.input.buffer();
    std::string message;
    for (; vm.user->TO_IN < vm.user->NR_IN; ++vm.user->TO_IN) {
        if (buffer[vm.user->TO_IN] == '\"') {
            ++vm.user->TO_IN;
            break;
        }

        switch (buffer[vm.user->TO_IN]) {
        case '\\':
            ++vm.user->TO_IN;
            if (vm.user->TO_IN < vm.user->NR_IN) {
                switch (buffer[vm.user->TO_IN]) {
                case 'a':
                    message.push_back('\a');
                    break;
                case 'b':
                    message.push_back('\b');
                    break;
                case 'e':
                    message.push_back('\x1B');
                    break;
                case 'f':
                    message.push_back('\f');
                    break;
                case 'l':
                    message.push_back('\n');
                    break;
                case 'm':
                    message.push_back('\r');
                    message.push_back('\n');
                    break;
                case 'n':
                    message.push_back('\n');
                    break;
                case 'q':
                    message.push_back('"');
                    break;
                case 'r':
                    message.push_back('\r');
                    break;
                case 't':
                    message.push_back('\t');
                    break;
                case 'v':
                    message.push_back('\v');
                    break;
                case 'z':
                    message.push_back('\0');
                    break;
                case '"':
                    message.push_back('"');
                    break;
                case '\\':
                    message.push_back('\\');
                    break;
                case 'x':
                    ++vm.user->TO_IN;
                    if (vm.user->TO_IN + 1 < vm.user->NR_IN &&
                            isxdigit(buffer[vm.user->TO_IN]) &&
                            isxdigit(buffer[vm.user->TO_IN + 1])
                       ) {
                        std::string hex_str = std::string(&buffer[vm.user->TO_IN],
                                                          &buffer[vm.user->TO_IN + 2]);
                        ++vm.user->TO_IN;
                        int char_value = std::stoi(hex_str, nullptr, 16);
                        message.push_back(char_value);
                    }
                    break;
                default:
                    break;  // ignore all other escape chars
                }
            }
            break;
        default:
            message.push_back(buffer[vm.user->TO_IN]);
        }
    }
    return message;
}

static bool parse_sign(const char*& p, const char* end, int& sign) {
    bool found_sign = false;
    while (p < end) {
        switch (*p) {
        case '-':
            found_sign = true;
            sign *= -1;
            p++;
            break;
        case '+':
            found_sign = true;
            sign *= 1;
            p++;
            break;
        default:
            return found_sign;
        }
    }
    return found_sign;
}

static int parse_digits(const char*& p, const char* end, int base,
                        dint& value) {
    int num_digits = 0;
    while (p < end) {
        int digit = char_digit(*p);
        if (digit >= 0 && digit < base) {
            ++p;
            ++num_digits;
            value = value * base + digit;
        }
        else {
            break;
        }
    }

    return num_digits;
}

bool parse_number(const std::string& text, bool& is_double, dint& value) {
    return parse_number(text.c_str(), static_cast<uint>(text.size()), is_double,
                        value);
}

// parse number with optional sign (+-)
// accept $ # as hex prefixes, % as binary prefix, 'a' as quoted character
// skip punctuation ( , . + - / : )
// if punctuation found, set DPL to number of digits after last punctuation, return double cell
// return true if ok, false if error
bool parse_number(const char* text, uint size, bool& is_double, dint& value) {
    static const std::string punctuation = ",.+-/:";

    // init output vars
    int sign = 1;
    int base = vm.user->BASE;
    vm.user->DPL = 0;
    const char* p = text;
    const char* end = text + size;
    is_double = false;
    value = 0;

    // check sign before number prefix
    parse_sign(p, end, sign);

    if (p >= end) {
        return false;    // no number
    }

    // check base prefix
    switch (*p) {
    case '#':
        base = 10;
        p++;
        break;
    case '$':
        base = 16;
        p++;
        break;
    case '%':
        base = 2;
        p++;
        break;
    case '\'':
        if (end - p == 3 && p[2] == '\'') {
            value = sign * static_cast<unsigned char>(p[1]);
            is_double = false;
            return true;
        }
        break;
    }

    // check sign after number prefix
    parse_sign(p, end, sign);

    // collect digits
    bool found_digits = false;
    while (p < end) {
        int num_digits = parse_digits(p, end, base, value);
        if (num_digits > 0) {
            found_digits = true;
            if (is_double) {
                vm.user->DPL += num_digits;
            }
        }
        else if (punctuation.find(*p) != std::string::npos) {
            ++p;
            vm.user->DPL = 0;
            is_double = true;
        }
        else {
            return false;    // digit not in BASE
        }
    }

    if (!found_digits) {
        return false;    // no digits found
    }

    value *= sign;
    return true;
}

bool parse_float(const std::string& text, double& value, bool needs_exp) {
    return parse_float(text.c_str(), static_cast<uint>(text.size()), value,
                       needs_exp);
}

bool parse_float(const char* text, uint size, double& value, bool needs_exp) {
    value = 0.0;
    if (vm.user->BASE != 10) {
        return false;
    }

    const char* p = text;
    const char* end = text + size;
    int sign = 1;
    int exp_sign = 1;
    dint dummy = 0;

    // start with a sign, digits*, '.'?, digits*
    parse_sign(p, end, sign);

    const char* start_mantissa = p;
    int num_digits = parse_digits(p, end, 10, dummy);
    if (p < end && *p == '.') {
        ++p;
        num_digits += parse_digits(p, end, 10, dummy);
    }
    const char* end_mantissa = p;

    if (num_digits == 0) {
        return false;
    }

    // check for mandatory exponent
    if (needs_exp && p >= end) {
        return false;
    }

    // optional exponent 'e' or 'd', sign?, digits+
    if (p >= end) {
        std::string number = sign < 0 ? "-" : "";
        number += std::string(start_mantissa, end_mantissa);
        value = std::stod(number);
        return true;
    }
    else {
        if (toupper(*p) != 'D' && toupper(*p) != 'E') {
            return false;
        }

        ++p;
        bool has_sign = parse_sign(p, end, exp_sign);

        const char* start_exponent = p;
        int num_digits_exp = parse_digits(p, end, 10, dummy);
        const char* end_exponent = p;

        if (has_sign && num_digits_exp == 0) {
            return false;    // exponent only with sign
        }
        else if (p < end) {
            return false;    // extra characters after number
        }
        else {
            std::string number = sign < 0 ? "-" : "";
            number += std::string(start_mantissa, end_mantissa);
            if (num_digits_exp > 0) {
                number += "e";
                number += exp_sign < 0 ? "-" : "";
                number += std::string(start_exponent, end_exponent);
            }
            value = std::stod(number);
            return true;
        }
    }
}

int f_word(char delimiter) {
    CString* word = parse_cword(delimiter);
    return mem_addr(word);
}

void f_parse(char delimiter) {
    uint size;
    const char* word = parse_word(size, delimiter);
    push(mem_addr(word));
    push(size);
}

void f_parse_name() {
    uint size;
    const char* word = parse_word(size, BL);
    push(mem_addr(word));
    push(size);
}

int f_char(char delimiter) {
    uint size = 0;
    const char* str = parse_word(size, delimiter);
    if (size == 0) {
        return 0;
    }
    else {
        return *str;
    }
}

void f_bracket_char(char delimiter) {
    int c = f_char(delimiter);
    comma(xtXLITERAL);
    comma(c);
}

static int _number(bool do_error) {
    uint size = pop();
    uint addr = pop();
    dint value;
    bool is_double;
    if (parse_number(mem_char_ptr(addr, size), size, is_double, value)) {
        if (is_double) {
            push(dcell_lo(value));
            push(dcell_hi(value));
            return 2;
        }
        else {
            push(dcell_lo(value));
            return 1;
        }
    }
    else {
        if (do_error)
            error(Error::InvalidNumericArgument, std::string(mem_char_ptr(addr, size),
                    mem_char_ptr(addr, size) + size));
        return 0;
    }
}

void f_number_q() {
    int num_cells = _number(false);
    push(num_cells);
}

void f_number() {
    _number(true);
}

void f_to_number() {
    uint size = pop();
    uint addr = pop();
    udint n = (udint)dpop();
    int digit;
    while (size > 0 && (digit = char_digit(cfetch(addr))) >= 0
            && digit < vm.user->BASE) {
        n = n * vm.user->BASE + digit;
        addr++;
        size--;
    }
    dpush(n);
    push(addr);
    push(size);
}

void f_convert() {
    uint addr = pop() + 1;
    udint n = (udint)dpop();
    int digit;
    while ((digit = char_digit(cfetch(addr))) >= 0 &&
            digit < vm.user->BASE) {
        n = n * vm.user->BASE + digit;
        addr++;
    }
    dpush(n);
    push(addr);
}

void f_open_paren() {
    if (vm.input.source_id() != 0) {
        bool found = false;
        while (!found) {
            skip_to_delimiter(')', found);
            if (found) {
                break;
            }
            else if (vm.input.refill()) {
                continue;
            }
            else {
                break;
            }
        }
    }
    else {
        bool found;
        skip_to_delimiter(')', found);
    }
}

void f_backslash() {
    if (vm.user->BLK > 0) {
        vm.user->TO_IN = (vm.user->TO_IN + BLOCK_COLS - 1) & ~(BLOCK_COLS - 1);
    }
    else {
        bool found;
        skip_to_delimiter(CR, found);
    }
}

void f_to_float() {
    uint size = pop();
    uint addr = pop();
    const char* str = mem_char_ptr(addr, size);
    strip_blanks(str, size);
    double value = 0.0;
    if (size == 0 || parse_float(str, size, value, false)) {
        fpush(value);
        push(F_TRUE);
    }
    else {
        push(F_FALSE);
    }
}
