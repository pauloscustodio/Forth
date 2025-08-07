//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "parser.h"
#include "vm.h"
using namespace std;

// ignore all control characters as spaces
static bool is_space(char c) {
    return c >= 0 && c <= BL;
}

// return digit value of character, or -1 if not a digit
static int char_digit(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else
        return -1; // not a digit
}

static void skip_blanks() {
    char* buffer = vm.input->buffer();

    while (vm.user->TO_IN < vm.user->NR_IN  && is_space(buffer[vm.user->TO_IN]))
        ++vm.user->TO_IN;
}

static int skip_to_delimiter(char delimiter) {
    char* buffer = vm.input->buffer();

    int end = vm.user->TO_IN;
    if (delimiter == BL) {
        while (vm.user->TO_IN < vm.user->NR_IN && !is_space(buffer[vm.user->TO_IN]))
            ++vm.user->TO_IN;
        
        end = vm.user->TO_IN;

        if (vm.user->TO_IN < vm.user->NR_IN && is_space(buffer[vm.user->TO_IN]))
            ++vm.user->TO_IN;		// skip delimiter
    }
    else {
        while (vm.user->TO_IN < vm.user->NR_IN && buffer[vm.user->TO_IN] != delimiter)
            ++vm.user->TO_IN;
        
        end = vm.user->TO_IN;

        if (vm.user->TO_IN < vm.user->NR_IN && buffer[vm.user->TO_IN] == delimiter)
            ++vm.user->TO_IN;		// skip delimiter
        
    }

    return end;	// end of word, char before delimiter
}

const char* parse_word(int& size, char delimiter) {
    if (delimiter == BL)
        skip_blanks();	// skip blanks before word

    char* buffer = vm.input->buffer();
    int start = vm.user->TO_IN;
    int end = skip_to_delimiter(delimiter);

    size = end - start;
    const char* word = &buffer[start];
    return word;
}

CString* parse_cword(char delimiter) {
    int size = 0;
    const char* word = parse_word(size, delimiter);
    CString* cword = vm.wordbuf->append(word, size);
    return cword;
}

bool parse_number(const string& text, bool& is_double, dint& value) {
    return parse_number(text.c_str(), text.size(), is_double, value);
}

bool parse_number(const char* text, size_t size, bool& is_double, dint& value) {
    return parse_number(text, static_cast<int>(size), is_double, value);
}

// parse number with optional sign (+-)
// accept $ # as hex prefixes, % as binary prefix, 'a' as quoted character
// skip punctuation ( , . + - / : )
// if punctuation found, set DPL to number of digits after last punctuation, return double cell
// return true if ok, false if error
bool parse_number(const char* text, int size, bool& is_double, dint& value) {
    // init output vars
    int sign = 1;
    int base = vm.user->BASE;
    vm.user->DPL = 0;
    const char* p = text;
    const char* end = text + size;
    is_double = false;
    value = 0;

    if (p >= end) return false;			// no number

    // check sign
    switch (*p) {
    case '-':
        sign = -1;
        p++;
        break;
    case '+':
        sign = 1;
        p++;
        break;
    }

    if (p >= end) return false;			// no number

    // check base prefix
    switch (*p) {
    case '#':
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

    if (p >= end) return false;			// no number

    // collect digits
    int num_digits = 0, digit = 0;
    is_double = false;
    while (p < end) {
        char c = *p++;
        switch (c) {
        case ',':
        case '.':
        case '+':
        case '-':
        case '/':
        case ':':
            vm.user->DPL = 0;
            is_double = true;
            break;
        default:
            digit = char_digit(c);
            if (digit < 0 || digit >= base)
                return false;		// digit not in BASE

            value = value * base + digit;
            num_digits++;
            if (is_double)
                vm.user->DPL++;
        }
    }
    if (num_digits == 0)
        return false;               // no digits found

    value *= sign;
    return true;
}

int f_word(char delimiter) {
    CString* word = parse_cword(delimiter);
    return mem_addr(word);
}

void f_parse(char delimiter) {
    int size;
    const char* word = parse_word(size, delimiter);
    push(mem_addr(word));
    push(size);
}

void f_parse_name() {
    int size;
    const char* word = parse_word(size, BL);
    push(mem_addr(word));
    push(size);
}

int f_char(char delimiter) {
    int size = 0;
    const char* str = parse_word(size, delimiter);
    if (size == 0)
        return 0;
    else
        return *str;
}

void f_bracket_char(char delimiter) {
    int c = f_char(delimiter);
    comma(xtXLITERAL);
    comma(c);
}

static int _number(bool do_error) {
    int size = pop();
    int addr = pop();
    dint value;
    bool is_double;
    if (parse_number(mem_char_ptr(addr), size, is_double, value)) {
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
            error(Error::InvalidNumber, string(mem_char_ptr(addr), mem_char_ptr(addr) + size));
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
    int size = pop();
    int addr = pop();
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
    int addr = pop() + 1;
    udint n = (udint)dpop();
    int digit;
    while ((digit = char_digit(cfetch(addr))) >= 0 && digit < vm.user->BASE) {
        n = n * vm.user->BASE + digit;
        addr++;
    }
    dpush(n);
    push(addr);
}

void f_open_paren() {
    skip_to_delimiter(')');
}

void f_backslash() {
    /* TODO: skip to end of block line
    if (vm.input->blk() != 0) {
        int* to_in = vm.input->to_in_ptr();
        *to_in = (*to_in + BLOCK_COLS - 1) & ~(BLOCK_COLS - 1);
    }
    */

    skip_to_delimiter(CR);
}

