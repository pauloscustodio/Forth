//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>
using namespace std;

// types
typedef uint8_t uchar;
typedef unsigned int uint;
typedef int64_t dint;
typedef uint64_t udint;

// types sizes
static const int CHAR_SZ = sizeof(char);
static const int CELL_SZ = sizeof(int);
static const int DCELL_SZ = sizeof(dint);

static_assert(CELL_SZ * 2 == DCELL_SZ, "DCELL should be double of CELL");

// size of the virtual machine
static const int MEM_SZ = 256 * 1024 * CELL_SZ;
static const int BUFFER_SZ = 1024;
static const int WORDBUF_SZ = BUFFER_SZ;
static const int PAD_SZ = 256;
static const int STACK_SZ = 1024 * CELL_SZ;
static const int MAX_CSTRING_SZ = 0xff;
static const int MAX_NAME_SZ = 0x3f;

// constants
static const char BL = ' ';
static const char CR = '\n';
static const int SCREEN_WIDTH = 80;

static const int F_TRUE = -1;
static const int F_FALSE = 0;

static const int F_SMUDGE = 0x20;
static const int F_HIDDEN = 0x40;
static const int F_IMMEDIATE = 0x80;

static const int STATE_INTERPRET = 0;
static const int STATE_COMPILE = 1;

// blocks
static const string BLOCKS_FILE = "blocks.fb";
static const int BLOCK_SZ = 1024;
static const int BLOCK_ROWS = 16;
static const int BLOCK_COLS = 64;
static const int NUM_BLK_BUFFERS = 16;

// idWORD for all words - used in switch statement to select word to execute
enum {
#define CONST(word, name, flags, value) id##name,
#define VAR(word, name, flags, value)   id##name,
#define CODE(word, name, flags, c_code) id##name,
#define FORTH(word, name, flags, text)  id##name,
#include "words.def"
};

// declare xtWORD for all words - execution token from dictionary
#define CONST(word, name, flags, value) extern int xt##name;
#define VAR(word, name, flags, value)   extern int xt##name;
#define CODE(word, name, flags, c_code) extern int xt##name;
#define FORTH(word, name, flags, text)  extern int xt##name;
#include "words.def"

// bool
int f_bool(bool f);

// alignment and double cells
int aligned(int x);
int dcell_lo(dint x);
int dcell_hi(dint x);
dint dcell(int hi, int lo);

// user variables
struct User {
#define VAR(word, name, flags, value)   int name;
#include "words.def"

    void init();
};

// create dictionary
void create_dictionary();

// inner interpreter
void f_execute(int xt);
