//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "parser.h"
#include "vm.h"
#include <cassert>
using namespace std;

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

// ignore all control characters as spaces
static bool is_space(char c) {
    return c >= 0 && c <= BL;
}

static void skip_blank() {
    char* tib = vm.input->source_ptr();
    int nr_in = vm.input->nr_in();
    int* to_in = vm.input->to_in_ptr();

    if (*to_in < nr_in && is_space(tib[*to_in])) {
        ++*to_in;
    }
}

static void skip_blanks() {
    char* tib = vm.input->source_ptr();
    int nr_in = vm.input->nr_in();
    int* to_in = vm.input->to_in_ptr();

    while (*to_in < nr_in && is_space(tib[*to_in])) {
        ++*to_in;
    }
}

static int skip_to_delimiter(char delimiter) {
    char* tib = vm.input->source_ptr();
    int nr_in = vm.input->nr_in();
    int* to_in = vm.input->to_in_ptr();

    int end = *to_in;
    if (delimiter == BL) {
        while (*to_in < nr_in && !is_space(tib[*to_in])) {
            ++*to_in;
        }
        end = *to_in;
    }
    else {
        while (*to_in < nr_in && tib[*to_in] != delimiter) {
            ++*to_in;
        }
        end = *to_in;
        if (*to_in < nr_in && tib[*to_in] == delimiter) {
            ++*to_in;		// skip delimiter
        }
    }

    return end;	// end of word, char before delimiter
}

// parse a word from the input buffer, delimited by the specified character
const ForthString* parse_word(char delimiter) {
    if (delimiter == BL)
        skip_blanks();	// skip blanks before word
    else
        skip_blank();	// skip space after quote

    char* tib = vm.input->source_ptr();
    int start = vm.input->to_in();
    int end = skip_to_delimiter(delimiter);

    int size = end - start;
    ForthString* ret = vm.wordbuf->append(tib + start, size);
    return ret;
}

void f_parse() {
    char delimiter = pop();
    const ForthString* word = parse_word(delimiter);
    push(mem_addr(word->str()));
    push(word->size());
}

void f_parse_word() {
    const ForthString* word = parse_word(BL);
    push(mem_addr(word->str()));
    push(word->size());
}

void f_word() {
    char delimiter = pop();
    const ForthString* word = parse_word(delimiter);
    const CountedString* addr = word->counted_string();
    push(mem_addr(addr));	// address counted string
}

void f_char() {
    const ForthString* word = parse_word(BL);
    if (word->size() == 0)
        push(0);
    else
        push(word->str()[0]);
}

void f_bracket_char() {
    f_char();
    char c = pop();
    comma(xtXLITERAL);
    comma(c);
}

bool parse_number(const string& text, bool& is_double, dint& value) {
    return parse_number(text.c_str(), text.size(), is_double, value);
}

// parse number with optional sign (+-)
// accept $ # as hex prefixes, % as binary prefix, 'a' as quoted character
// skip punctuation ( , . + - / : )
// if punctuation found, set DPL to number of digits after last punctuation, return double cell
// return true if ok, false if error
bool parse_number(const char* text, size_t size, bool& is_double, dint& value) {
    return parse_number(text, static_cast<int>(size), is_double, value);
}

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

static int _number(bool do_error) {
    int size = pop();
    int addr = pop();
    const char* text = mem_char_ptr(addr);
    bool is_double;
    dint value;

    if (!parse_number(text, size, is_double, value)) {
        if (do_error)
            error(Error::InvalidNumber, string(text, text + size));
        return 0;
    }
    else {
        if (is_double) {
            push(dcell_lo(value));
            push(dcell_hi(value));
            return 2;
        }
        else {
            push(dcell_lo(value));
        }
        return 1;
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
    udint n = static_cast<udint>(dpop());
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
    udint n = static_cast<udint>(dpop());
    int digit;
    while ((digit = char_digit(cfetch(addr))) >= 0 && digit < vm.user->BASE) {
        n = n * vm.user->BASE + digit;
        addr++;
    }
    dpush(n);
    push(addr);
}
