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
static inline const int CHAR_SZ = static_cast<int>(sizeof(char));
static inline const int CELL_SZ = static_cast<int>(sizeof(int));
static inline const int DCELL_SZ = static_cast<int>(sizeof(dint));
static inline const int PTR_SZ = static_cast<int>(sizeof(func_ptr_t));

static_assert(CELL_SZ * 2 == DCELL_SZ, "DCELL should be double of CELL");
static_assert(PTR_SZ <= DCELL_SZ, "PTR should fit in DCELL");

// VM size
static inline const int MEM_SZ = 256 * 1024 * CELL_SZ;
static inline const int BUFFER_SZ = 1024;
static inline const int PAD_SZ = 256;
static inline const int STACK_SZ = 1024 * CELL_SZ;
static inline const int MAX_FILES = 16;

// constants
static inline const char BL = ' ';
static inline const char CR = '\n';
static inline const int SCREEN_WIDTH = 80;

static inline const int F_TRUE = -1;
static inline const int F_FALSE = 0;

static inline const int MAX_WORD_SZ = 0xff;
static inline const int F_SMUDGE = 0x1;
static inline const int F_HIDDEN = 0x2;
static inline const int F_IMMEDIATE = 0x4;

static inline const int STATE_INTERPRET = 0;
static inline const int STATE_COMPILE = 1;

// file names
static inline const string BLOCK_FILENAME = "forth.blk";

// alignment and double cells
int aligned(int x);
int dcell_lo(dint x);
int dcell_hi(dint x);
int dcell(int hi, int lo);

void push(int value);
int pop();
int peek(int depth = 0);

void dpush(dint value);
dint dpop();
dint dpeek(int depth = 0);

void rpush(int value);
int rpop();
int rpeek(int depth = 0);

void fVOID();

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
extern int xtTYPE; // TYPE
extern int xtEMIT; // EMIT
extern int xtCR; // CR
extern int xtSPACE; // SPACE
extern int xtSPACES; // SPACES
extern int xtLESS_HASH; // <#
extern int xtHASH; // #
extern int xtHASH_S; // #S
extern int xtHOLD; // HOLD
extern int xtSIGN; // SIGN
extern int xtHASH_GREATER; // #>
extern int xtDOT; // .
extern int xtD_DOT; // D.
extern int xtD_DOT_R; // D.R
extern int xtU_DOT; // U.
extern int xtDOT_R; // .R
extern int xtU_DOT_R; // U.R
extern int xtPAD; // PAD
extern int xtTHROW; // THROW
extern int xtDROP; // DROP
extern int xtDUP; // DUP
extern int xtPICK; // PICK
extern int xtPLUS; // +
extern int xtENVIRONMENT_Q; // ENVIRONMENT?
extern int xtCOUNT; // COUNT
extern int xtS_QUOTE; // S"
extern int xtDOT_S; // .S
extern int xtWORD; // WORD
extern int xtWORDS; // WORDS
extern int xtFIND; // FIND
extern int xtCOMMA; // ,
extern int xtC_COMMA; // C,
extern int xtALIGN; // ALIGN
//@@END

//@@BEGIN: WordsDeclaration
void fBASE(); // BASE
void fSTATE(); // STATE
void fSTORE(); // !
void fFETCH(); // @
void fC_STORE(); // C!
void fC_FETCH(); // C@
void fTYPE(); // TYPE
void fEMIT(); // EMIT
void fCR(); // CR
void fSPACE(); // SPACE
void fSPACES(); // SPACES
void fLESS_HASH(); // <#
void fHASH(); // #
void fHASH_S(); // #S
void fHOLD(); // HOLD
void fSIGN(); // SIGN
void fHASH_GREATER(); // #>
void fDOT(); // .
void fD_DOT(); // D.
void fD_DOT_R(); // D.R
void fU_DOT(); // U.
void fDOT_R(); // .R
void fU_DOT_R(); // U.R
void fPAD(); // PAD
void fTHROW(); // THROW
void fDROP(); // DROP
void fDUP(); // DUP
void fPICK(); // PICK
void fPLUS(); // +
void fENVIRONMENT_Q(); // ENVIRONMENT?
void fCOUNT(); // COUNT
void fS_QUOTE(); // S"
void fDOT_S(); // .S
void fWORD(); // WORD
void fWORDS(); // WORDS
void fFIND(); // FIND
void fCOMMA(); // ,
void fC_COMMA(); // C,
void fALIGN(); // ALIGN
//@@END
