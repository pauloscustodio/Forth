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

#if 0
// return digit value of character, or -1 if not a digit
static int digit_value(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else
        return -1; // not a digit
}

static int digit_value(char c, int base) {
    int value = digit_value(c);
    return (value >= 0 && value < base) ? value : -1; // valid digit for the base
}
#endif 

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
