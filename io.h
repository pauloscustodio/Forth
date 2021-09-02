//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "memory.h"
#include <stdio.h>
#include <stdbool.h>

#define BL              ' '
#define CR              '\n'
#define SCREEN_WIDTH    80

// command line args
extern bool     interactive;
extern int      cmd_argc;
extern char**   cmd_argv;

void init_io(void);

Buffer* cur_buffer(void);

void push_stream(FILE* file);
void push_text(const char* text, int len);
void push_file(const char* filename, int len);
void pop_input(void);

int f_tib(void);
int f_nr_in(void);
int f_to_in(void);

bool f_refill(void);
void f_query(void);

enum { INPUT_EVAL_KBD, INPUT_FILE };

void f_save_input(void);
void f_restore_input(void);

// pictured output
void numout_start(void);
void numout_digit(void);
void numout_digits(void);
void numout_char(char c);
void numout_sign(int n);
void numout_end(void);

void print_int(int n);
void print_uint(int n);
void print_int_aligned(int n, int width);
void print_uint_aligned(int n, int width);

void f_emit(int c);
void f_spaces(int n);
void f_type(void);

void f_accept(void);
void f_expect(void);
int f_key(void);
