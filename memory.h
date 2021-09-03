//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <stdbool.h>
#include <stdio.h>

// VM size
#define MEM_SZ          (256 * 1024)
#define BUFFER_SZ       1024
#define MAX_FILES       16
#define MAX_BLOCKS      16
#define PAD_SZ          256
#define HOLD_SZ         128
#define WORDBUF_SZ      256
#define STACK_SZ        (16 * 1024)
#define STRUCT_STACK_SZ 1024

// user variables
typedef struct User {
    char    pad[PAD_SZ];
    char    hold[HOLD_SZ];
    char    wordbuf[WORDBUF_SZ];

    int     sp, rp, struct_p;       // stack pointers
    int     here, latest;           // dictionary pointers
    int     source_id, blk, scr;    // input source
    int     hold_ptr;               // point to hold[]
    bool    in_colon;               // inside colon definition

#define VAR(word, name, flags, value)   \
        int name;
#include "words.def"
} User;

// input buffers
typedef struct Buffer {
    FILE*   file;
    int     blk;            // block, if any
    bool    dirty;          // true if block is dirty
    int     nr_in;          // length
    int     to_in;          // input cursor
    char    tib[BUFFER_SZ + 1]; // Forth buffer + space for C's null terminator
} Buffer;

typedef struct Input {
    Buffer  buff[MAX_FILES];
    int     buff_p;
    Buffer  block[MAX_BLOCKS];
    int     block_p;        // last block allocated
    Buffer* last_block;     // last block accessed
} Input;

// memory addresses
#define PTR00       MEM_SZ
#define PTR01       (PTR00 - sizeof(Input))     // input buffers
#define PTR02       (PTR01 - STACK_SZ)          // return stack
#define PTR03       (PTR02 - STRUCT_STACK_SZ)   // struct stack
#define PTR04       (PTR03 - STACK_SZ)          // data stack

#define INPUT0      PTR01
#define R0          PTR01
#define R1          PTR02
#define STRUCT0     PTR02
#define STRUCT1     PTR03
#define S0          PTR03
#define S1          PTR04
#define DICT_SZ     PTR04

// VM memory
extern char mem[];
extern User* user;
extern Input* input;

void init_memory(void);
int  fetch(int addr);
void store(int addr, int value);
dint dfetch(int addr);
void dstore(int addr, dint value);
int  cfetch(int addr);
void cstore(int addr, int value);

const char* fstr_to_cstr(const char* text, int len);
int         fstr_to_counted_str(const char* text, int len);

int         counted_str_size(int len);
const char* counted_str_fetch(int addr, int* len);
void        counted_str_store(int addr, const char* text, int len);

int         dict_name_size(int len);
const char* dict_name_fetch(int addr, int* len);
void        dict_name_store(int addr, const char* text, int len, int flags);

int         long_str_size(int len);
const char* long_str_fetch(int addr, int* len);
void        long_str_store(int addr, const char* text, int len);

void f_fill(void);
void f_erase(void);
void f_move(void);

