//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "io.h"

#define BLOCK_ROWS  16
#define BLOCK_COLS  64

Buffer* get_block(int blk);
bool get_file_block(int blk);
int f_block(int blk);
int f_buffer(int blk);
void f_save_buffers(void);
void f_empty_buffers(void);
void f_flush(void);
void f_update(void);
void f_list(int blk);
void f_thru(void);
