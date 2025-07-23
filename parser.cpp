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
    const char* buffer = vm.input->buffer();
    int ptr = vm.input->buffer_ptr();
    int size = vm.input->buffer_size();

    if (ptr < size && is_space(buffer[ptr])) {
        ++ptr; // skip spaces
    }

    vm.input->set_buffer_ptr(ptr); // update buffer pointer
}

static void skip_blanks() {
    const char* buffer = vm.input->buffer();
    int ptr = vm.input->buffer_ptr();
    int size = vm.input->buffer_size();

    while (ptr < size && is_space(buffer[ptr])) {
        ++ptr; // skip spaces
    }

    vm.input->set_buffer_ptr(ptr); // update buffer pointer
}

static int skip_to_delimiter(char delimiter = BL) {
    const char* buffer = vm.input->buffer();
    int ptr = vm.input->buffer_ptr();
    int size = vm.input->buffer_size();

    int end = size;
    if (delimiter == BL) {
        while (ptr < size && !is_space(buffer[ptr]))
            ++ptr;
        end = ptr;
    }
    else {
        while (ptr < size && buffer[ptr] != delimiter)
            ++ptr;
        end = ptr;
        if (ptr < size && buffer[ptr] == delimiter)
            ++ptr;		// skip delimiter
    }

    vm.input->set_buffer_ptr(ptr); // update buffer pointer
    return end;	// end of word, char before delimiter
}

// parse a word from the input buffer, delimited by the specified character
static CountedString* parse_word_1(char delimiter) {
    while (true) {
        const char* buffer = vm.input->buffer();
        int ptr = vm.input->buffer_ptr();
        int size = vm.input->buffer_size();

        if (ptr >= size) {		        // end of buffer
            if (!vm.input->refill())		// try to read next line
                return nullptr;		    // EOF
            else
                continue;			    // try again
        }

        if (delimiter == BL)
            skip_blanks();	// skip blanks before word
        else
            skip_blank();	// skip space after quote

        ptr = vm.input->buffer_ptr();
        int start = ptr;
        int end = skip_to_delimiter(delimiter);

        if (delimiter == BL && start == end) {
            // no word found, continue to next line
            continue;
        }
        else {
            int size = end - start;
            CountedString* ret = vm.wordbuf->append(buffer + start, size);
            return ret;
        }
    }
}

CountedString* parse_word(char delimiter) {
    while (vm.input->has_input()) {
        CountedString* word = parse_word_1(delimiter);
        if (word != nullptr) {		            // found a word
            return word;
        }
        else if (vm.input->source_id() < 0) {   // input from string
            return nullptr;		                // no more words
        }
        else { 
            vm.input->pop_input();	            // remove last buffer
        }
    }
    return nullptr;		                        // no input at all
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

CountedString* cWORD(char delimiter) {
    CountedString* word = parse_word(delimiter);
    if (word == nullptr)
        exit(EXIT_SUCCESS);		// no more input
    else
        return word;				// valid word
}

void fWORD() {
    char delimiter = pop();
    CountedString* word = cWORD(delimiter);
    assert(word != nullptr);

    push(vm.mem.addr(word->str));			// address of word
    push(word->size);						// length of word
}

void fS_QUOTE() {
    CountedString* word = cWORD('"');
    assert(word != nullptr);

    push(vm.mem.addr(word->str));			// address of word
    push(word->size);						// length of word
}
