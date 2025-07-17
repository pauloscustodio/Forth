//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
using namespace std;

// types
typedef uint8_t uchar;
typedef unsigned int uint;
typedef int64_t dint;
typedef uint64_t	 udint;
typedef void (*func_ptr_t)();

// types sizes
inline static const int CHAR_SZ = static_cast<int>(sizeof(char));
inline static const int CELL_SZ = static_cast<int>(sizeof(int));
inline static const int DCELL_SZ = static_cast<int>(sizeof(dint));
inline static const int PTR_SZ = static_cast<int>(sizeof(func_ptr_t));

static_assert(CELL_SZ * 2 == DCELL_SZ, "DCELL should be double of CELL");
static_assert(PTR_SZ <= DCELL_SZ, "PTR should fit in DCELL");

// VM size
inline static const int MEM_SZ = 256 * 1024 * CELL_SZ;
inline static const int BUFFER_SZ = 1024;
inline static const int PAD_SZ = 256;
inline static const int WORDBUF_SZ = BUFFER_SZ;
inline static const int TIB_SZ = BUFFER_SZ;
inline static const int STACK_SZ = 1024 * CELL_SZ;
inline static const int MAX_WORD_SZ = 255;

// constants
inline static const char BL = ' ';
inline static const char CR = '\n';
inline static const int SCREEN_WIDTH = 80;

inline static const int F_TRUE = -1;
inline static const int F_FALSE = 0;

inline static const int F_HIDDEN = 0x40;
inline static const int F_IMMEDIATE = 0x80;

// alignment and double cells
int align(int x);
int dcell_lo(dint x);
int dcell_hi(dint x);
int dcell(int hi, int lo);

void push(int value);
int pop();
int peek(int depth = 0);

void rpush(int value);
int rpop();
int rpeek(int depth = 0);

char* c_WORD(char delimiter = BL);

//@@BEGIN: WordsDeclarations
void f_STORE(); // !
void f_FETCH(); // @
void f_C_STORE(); // C!
void f_C_FETCH(); // C@
void f_PAD(); // PAD
void f_DOT(); // .
void f_THROW(); // THROW
void f_DROP(); // DROP
void f_DUP(); // DUP
void f_PICK(); // PICK
void f_PLUS(); // +
void f_WORDS(); // WORDS
void f_ENVIRONMENT_Q(); // ENVIRONMENT?
void f_TYPE(); // TYPE
void f_COUNT(); // COUNT
void f_S_QUOTE(); // S"
void f_DOT_S(); // .S
void f_WORD(); // WORD
//@@END
