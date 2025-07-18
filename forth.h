//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>
#include <vector>
using namespace std;

// types
typedef uint8_t uchar;
typedef unsigned int uint;
typedef int64_t dint;
typedef uint64_t udint;
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

// constants
inline static const char BL = ' ';
inline static const char CR = '\n';
inline static const int SCREEN_WIDTH = 80;

inline static const int F_TRUE = -1;
inline static const int F_FALSE = 0;

inline static const int MAX_WORD_SZ = 0xff;
inline static const int F_SMUDGE = 0x100;
inline static const int F_HIDDEN = 0x200;
inline static const int F_IMMEDIATE = 0x400;

inline static const int STATE_INTERPRET = 0;
inline static const int STATE_COMPILE = 1;

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

struct CountedString;
CountedString* cWORD(char delimiter = BL);

struct Header;
Header* cFIND(const char* name, bool& is_immediate);
Header* cFIND(const char* name, int size, bool& is_immediate);

vector<string> cWORDS();

bool case_insensitive_equal(const string& a, const string& b);

//@@BEGIN: WordsXtDeclaration
extern int xtBASE; // BASE
extern int xtSTATE; // STATE
extern int xtSTORE; // !
extern int xtFETCH; // @
extern int xtC_STORE; // C!
extern int xtC_FETCH; // C@
extern int xtPAD; // PAD
extern int xtDOT; // .
extern int xtTHROW; // THROW
extern int xtDROP; // DROP
extern int xtDUP; // DUP
extern int xtPICK; // PICK
extern int xtPLUS; // +
extern int xtENVIRONMENT_Q; // ENVIRONMENT?
extern int xtTYPE; // TYPE
extern int xtCOUNT; // COUNT
extern int xtS_QUOTE; // S"
extern int xtDOT_S; // .S
extern int xtWORD; // WORD
extern int xtWORDS; // WORDS
extern int xtFIND; // FIND
//@@END

//@@BEGIN: WordsDeclaration
void fBASE(); // BASE
void fSTATE(); // STATE
void fSTORE(); // !
void fFETCH(); // @
void fC_STORE(); // C!
void fC_FETCH(); // C@
void fPAD(); // PAD
void fDOT(); // .
void fTHROW(); // THROW
void fDROP(); // DROP
void fDUP(); // DUP
void fPICK(); // PICK
void fPLUS(); // +
void fENVIRONMENT_Q(); // ENVIRONMENT?
void fTYPE(); // TYPE
void fCOUNT(); // COUNT
void fS_QUOTE(); // S"
void fDOT_S(); // .S
void fWORD(); // WORD
void fWORDS(); // WORDS
void fFIND(); // FIND
//@@END
