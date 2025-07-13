//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "parser.h"
#include "block.h"
#include "dict.h"
#include "errors.h"
#include "interp.h"
#include "io.h"
#include "memory.h"
#include "stack.h"
#include <assert.h>
#include <ctype.h>

// skip blanks; treat all char codes less than 32 as blanks
void skip_blanks(void) {
    Buffer* buff = cur_buffer();
    char* p = buff->tib + buff->to_in;
    char* end = buff->tib + buff->nr_in;
    for (; p < end && *p <= BL; p++)
        ;
    buff->to_in = p - buff->tib;
}

const char* c_parse(int delim, int* len) {
    if (delim == BL)
        skip_blanks();

    Buffer* buff = cur_buffer();
    const char* p = buff->tib + buff->to_in;
    const char* end = buff->tib + buff->nr_in;

    *len = 0;
    if (p >= end) 							// no more input
        return p;

    const char* p1 = p;
    const char* p2 = end;
    for (; p < end; p++) {
        char c = *p;
        if (c < BL)
            c = BL;
        if (c == delim) {
            p2 = p++;						// skip end delimiter
            break;
        }
    }

    buff->to_in = p - buff->tib;
    *len = p2 - p1;
    return p1;
}

const char* c_parse_word(int* len) {
    return c_parse(BL, len);
}

void f_parse(int delim) {
    int len;
    const char* str = c_parse(delim, &len);
    push(str - mem);
    push(len);
}

void f_parse_word(void) {
    int len;
    const char* str = c_parse_word(&len);
    push(str - mem);
    push(len);
}

// parse word, return address of counted string
int f_word(int delim) {
    int len = 0;
    const char* str = c_parse(delim, &len);
    return fstr_to_counted_str(str, len);
}

int f_char(int delim) {
    int len = 0;
    const char* str = c_parse(delim, &len);
    if (len == 0)
        return 0;
    else
        return *str;
}

void f_bracket_char(int delim) {
    int c = f_char(delim);
    comma(xtXLITERAL);
    comma(c);
}

void f_open_paren(void) {
    int len;
    c_parse(')', &len);
}

void f_backslash(void) {
    if (user->blk != 0) {
        Buffer* buff = cur_buffer();
        buff->to_in = (buff->to_in + BLOCK_COLS - 1) & ~(BLOCK_COLS - 1);
    }
    else {
        int len;
        c_parse(CR, &len);
    }
}

static int char_digit(char c) {
    if (isdigit(c))
        return c - '0';
    else if (isalpha(c))
        return toupper(c) - 'A' + 10;
    else
        return -1;			// no digit
}

// parse number with optional sign (+-)
// accept $ # as hex prefixes, % as binary prefix, 'a' as quoted character
// skip punctuation ( , . + - / : )
// if punctuation found, set DPL to number of digits after last punctuation, return double cell
// return 0 for error, 1 if a single number was found, 2 if a double number was found
int c_parse_number(const char* text, int len, dint* value) {
    // init output vars
    int sign = 1, base = user->BASE;
    *value = 0;
    user->DPL = 0;
    const char* p = text;
    const char* end = text + len;
    if (p >= end) return 0;			// no number

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
    if (p >= end) return 0;			// no number

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
            *value = sign * (unsigned char)p[1];
            return 1;
        }
        break;
    }
    if (p >= end) return 0;			// no number

    // collect digits
    int num_digits = 0, digit;
    int num_cells = 1;
    while (p < end) {
        char c = *p++;
        switch (c) {
        case ',':
        case '.':
        case '+':
        case '-':
        case '/':
        case ':':
            user->DPL = 0;
            num_cells = 2;
            break;
        default:
            digit = char_digit(c);
            if (digit < 0 || digit >= base)
                return 0;			// digit not in BASE

            *value = *value * base + digit;
            num_digits++;
            if (num_cells > 1)
                user->DPL++;
        }
    }
    if (num_digits == 0)
        return 0;                   // no digits found

    *value *= sign;
    return num_cells;
}

static int _number(bool do_error) {
    int len = pop();
    int addr = pop();
    dint value;
    int num_cells = c_parse_number(mem + addr, len, &value);
    switch (num_cells) {
    case 0:
        if (do_error)
            error_arg(ErrorInvalidNumber, fstr_to_cstr(mem + addr, len));
        return 0;
    case 1:
        push(DCELL_LO(value));
        return 1;
    case 2:
        push(DCELL_LO(value));
        push(DCELL_HI(value));
        return 2;
    default:
        assert(0);
        return 0;
    }
}

void f_number_q(void) {
    int num_cells = _number(false);
    push(num_cells);
}

void f_number(void) {
    _number(true);
}

void f_to_number(void) {
    int len = pop();
    int addr = pop();
    udint n = (udint)dpop();
    int digit;
    while (len > 0 && (digit = char_digit(cfetch(addr))) >= 0
            && digit < user->BASE) {
        n = n * user->BASE + digit;
        addr++;
        len--;
    }
    dpush(n);
    push(addr);
    push(len);
}

void f_convert(void) {
    int addr = pop() + 1;
    udint n = (udint)dpop();
    int digit;
    while ((digit = char_digit(cfetch(addr))) >= 0 && digit < user->BASE) {
        n = n * user->BASE + digit;
        addr++;
    }
    dpush(n);
    push(addr);
}
