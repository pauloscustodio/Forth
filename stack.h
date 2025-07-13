//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "memory.h"

void init_stacks(void);

void push(int value);
int  pop(void);
int  peek(int depth);
void dpush(dint value);
dint dpop(void);
dint dpeek(int depth);
int  depth(void);
void roll(int depth);
void print_stack(void);

void rpush(int value);
int  rpop(void);
int  rpeek(int depth);
void rdpush(dint value);
dint rdpop(void);
dint rdpeek(int depth);
int  rdepth(void);
void print_rstack(void);

void struct_push(int value);
int  struct_pop(void);
int  struct_peek(int depth);
