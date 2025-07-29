//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "parser.h"
#include "vm.h"
#include <cassert>
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

static void skip_blank() {
    const char* tib = vm.tib->tib();

    if (vm.user->TO_IN < vm.user->NR_IN && is_space(tib[vm.user->TO_IN])) {
        ++vm.user->TO_IN; // skip spaces
    }
}

static void skip_blanks() {
    const char* tib = vm.tib->tib();

    while (vm.user->TO_IN < vm.user->NR_IN && is_space(tib[vm.user->TO_IN])) {
        ++vm.user->TO_IN; // skip spaces
    }
}

static int skip_to_delimiter(char delimiter = BL) {
    const char* tib = vm.tib->tib();

    int end = vm.user->TO_IN;
    if (delimiter == BL) {
        while (vm.user->TO_IN < vm.user->NR_IN && !is_space(tib[vm.user->TO_IN]))
            ++vm.user->TO_IN;
        end = vm.user->TO_IN;
    }
    else {
        while (vm.user->TO_IN < vm.user->NR_IN && tib[vm.user->TO_IN] != delimiter)
            ++vm.user->TO_IN;
        end = vm.user->TO_IN;
        if (vm.user->TO_IN < vm.user->NR_IN && tib[vm.user->TO_IN] == delimiter)
            ++vm.user->TO_IN;		// skip delimiter
    }

    return end;	// end of word, char before delimiter
}

// parse a word from the input buffer, delimited by the specified character
const ForthString* f_parse_word(char delimiter) {
    if (delimiter == BL)
        skip_blanks();	// skip blanks before word
    else
        skip_blank();	// skip space after quote

    const char* tib = vm.tib->tib();
    int start = vm.user->TO_IN;
    int end = skip_to_delimiter(delimiter);

    if (start == end) {
        return nullptr; // no word found
    }
    else {
        int size = end - start;
        ForthString* ret = vm.wordbuf->append(tib + start, size);
        return ret;
    }
}

// parse number with optional sign (+-)
// accept $ # as hex prefixes, % as binary prefix, 'a' as quoted character
// skip punctuation ( , . + - / : )
// if punctuation found, set DPL to number of digits after last punctuation, return double cell
// return true if ok, false if error
bool f_parse_number(const char* text, size_t size, bool& is_double, dint& value) {
    return f_parse_number(text, static_cast<int>(size), is_double, value);
}

bool f_parse_number(const char* text, int size, bool& is_double, dint& value) {
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

