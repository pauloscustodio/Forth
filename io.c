//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "io.h"
#include "errors.h"
#include "memory.h"
#include "stack.h"
#include "math.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMOUT_START    (user->hold + sizeof(user->hold) - mem)

// command line args
bool    interactive;
int     cmd_argc;
char**  cmd_argv;

void init_io(void) {
    input->buff_p = -1;
    push_stream(stdin);
    numout_start();
}

Buffer* cur_buffer(void) {
    return &input->buff[input->buff_p];
}

void push_stream(FILE* file) {
    if (input->buff_p + 1 >= MAX_FILES)
        error(ErrorTooManyOpenFiles);
    else {
        input->buff_p++;
        Buffer* buff = cur_buffer();
        buff->file = file;
        buff->nr_in = 0;
        buff->to_in = 0;
        user->source_id = input->buff_p;
    }
}

void push_text(const char* text, int len) {
    if (input->buff_p + 1 >= MAX_FILES)
        error(ErrorTooManyOpenFiles);
    else if (len > BUFFER_SZ)
        error(ErrorInputBufferOverflow);
    else {
        input->buff_p++;
        Buffer* buff = cur_buffer();
        buff->file = NULL;
        buff->nr_in = len;
        buff->to_in = 0;
        memcpy(buff->tib, text, len);
        user->source_id = -1;
    }
}

void push_file(const char* filename, int len) {
    if (input->buff_p + 1 >= MAX_FILES)
        error(ErrorTooManyOpenFiles);
    else {
        filename = fstr_to_cstr(filename, len);
        FILE* fp = fopen(filename, "r");
        if (!fp)
            error_arg(ErrorOpenFile, filename);
        else
            push_stream(fp);
    }
}

void pop_input(void) {
    if (input->buff_p > 0) {    // [0] is always stdin
        Buffer* buff = cur_buffer();
        if (buff->file != stdin && buff->file != NULL)
            fclose(buff->file);

        if (buff->to_in >= buff->nr_in)
            input->buff_p--;
        else
            buff->file = NULL;      // let refill pop this
    }
    user->source_id = input->buff_p;
}

int f_tib(void) {
    Buffer* buff = cur_buffer();
    return buff->tib - mem;
}

int f_nr_in(void) {
    Buffer* buff = cur_buffer();
    return (char*)&buff->nr_in - mem;
}

int f_to_in(void) {
    Buffer* buff = cur_buffer();
    return (char*)&buff->to_in - mem;
}

static void chomp(char* text) {
    int len = strlen(text);
    while (len > 0 && isspace(text[len - 1]))
        text[--len] = '\0';
}

// read input
bool f_refill(void) {
    if (user->source_id < 0)
        return false;
    else while (true) {
            Buffer* buff = cur_buffer();
            buff->nr_in = buff->to_in = 0;
            if (!interactive && input->buff_p == 0)
                return false;
            else if (buff->file && fgets(buff->tib, sizeof(buff->tib), buff->file)) {
                chomp(buff->tib);
                buff->nr_in = strlen(buff->tib);
                return true;
            }
            else if (input->buff_p > 0)
                pop_input();
            else
                return false;
        }
}

void f_query(void) {
    int old_source_id = user->source_id;    // save in case it's -1
    push_stream(stdin);
    f_refill();
    pop_input();
    user->source_id = old_source_id;
}

void f_save_input(void) {
    Buffer* buff = cur_buffer();
    if (buff->file == NULL || buff->file == stdin) {
        push(buff->to_in);
        push(INPUT_EVAL_KBD);
    }
    else {
        push(ftell(buff->file));
        push(buff->to_in);
        push(INPUT_FILE);
    }
}

void f_restore_input(void) {
    Buffer* buff = cur_buffer();
    switch (pop()) {
    case INPUT_EVAL_KBD:
        buff->to_in = pop();
        push(F_TRUE);
        break;
    case INPUT_FILE:
        buff->to_in = pop();
        fseek(buff->file, pop(), SEEK_SET);
        push(F_TRUE);
        break;
    default:
        push(F_FALSE);
    }
}

void numout_start(void) {
    user->hold_ptr = NUMOUT_START;
}

void numout_digit(void) {
    // compute digit
    dint n = dpop();
    int digit = (int)(n % user->BASE);    /* can use % instead of f_mod because n
                                             is assumed positive */
    n /= user->BASE;
    dpush(n);

    // output it
    if (digit < 10)
        numout_char('0' + digit);
    else
        numout_char('A' + digit - 10);
}

void numout_digits(void) {
    dint n;
    do {
        numout_digit();
        n = dpop();
        dpush(n);
    }
    while (n != 0);
}

void numout_char(char c) {
    char* p = mem + --user->hold_ptr;
    if (p < user->hold)
        error(ErrorNumberOutputOverflow);
    else
        *p = c;
}

void numout_sign(int n) {
    if (n < 0)
        numout_char('-');
}

void numout_end(void) {
    dpop();     // drop number
    push(user->hold_ptr);
    push(NUMOUT_START - user->hold_ptr);
}

static void print_int_uint(int sign) {
    numout_start();
    numout_char(BL);
    numout_digits();
    numout_sign(sign);
    numout_end();
    f_type();
}

void print_int(int n) {
    dpush(f_dabs(n));
    print_int_uint(n);
}

void print_uint(int n) {
    push(n);
    push(0);
    print_int_uint(1);
}

static void print_int_uint_aligned(int width, int sign) {
    numout_start();

    dint d;
    do {
        numout_digit();
        width--;
        d = dpop();
        dpush(d);
    }
    while (d != 0);

    if (sign < 0) {
        numout_sign(sign);
        width--;
    }

    while (width-- > 0)
        numout_char(BL);

    numout_end();
    f_type();
}

void print_int_aligned(int n, int width) {
    dpush(f_dabs(n));
    print_int_uint_aligned(width, n);
}

void print_uint_aligned(int n, int width) {
    push(n);
    push(0);
    print_int_uint_aligned(width, 1);
}

void f_emit(int c) {
    putchar(c);
}

void f_spaces(int n) {
    while (n-- > 0)
        f_emit(BL);
}

void f_type(void) {
    int len = pop();
    int addr = pop();
    if (len > 0)
        printf("%.*s", len, mem + addr);
}

void f_accept(void) {
    int maxlen = pop();
    char* addr = mem + pop();
    if (fgets(addr, maxlen, stdin)) {
        int len = strlen(addr);
        if (len > 0 && addr[len - 1] == '\n')   // remove newline
            addr[--len] = '\0';
        push(len);
    }
    else
        push(0);
}

void f_expect(void) {
    f_accept();
    user->SPAN = pop();
}

int f_key(void) {
    return getchar();
}
