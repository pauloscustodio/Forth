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
typedef void (*func_ptr_t)();

// types sizes
static inline const int CHAR_SZ = sizeof(char);
static inline const int CELL_SZ = sizeof(int);
static inline const int DCELL_SZ = sizeof(dint);
static inline const int PTR_SZ = sizeof(func_ptr_t);

static_assert(CELL_SZ * 2 == DCELL_SZ, "DCELL should be double of CELL");
static_assert(PTR_SZ <= DCELL_SZ, "PTR should fit in DCELL");

// VM size
static inline const int MEM_SZ = 256 * 1024 * CELL_SZ;
static inline const int BUFFER_SZ = 1024;
static inline const int WORDBUF_SZ = 2 * BUFFER_SZ;     // hold string with BUFFER_SZ length
static inline const int PAD_SZ = 256;
static inline const int STACK_SZ = 1024 * CELL_SZ;
static inline const int MAX_FILES = 16;
static inline const int MAX_COUNTED_STRING_SZ = 0xff;
static inline const int MAX_STRING_SZ = BUFFER_SZ;

// constants
static inline const char BL = ' ';
static inline const char CR = '\n';
static inline const int SCREEN_WIDTH = 80;

static inline const int F_TRUE = -1;
static inline const int F_FALSE = 0;

static inline const int F_SMUDGE = 0x1;
static inline const int F_HIDDEN = 0x2;
static inline const int F_IMMEDIATE = 0x4;

static inline const int STATE_INTERPRET = 0;
static inline const int STATE_COMPILE = 1;

// file names
static inline const string BLOCK_FILENAME = "forth.blk";

enum {
    //@@BEGIN: WordsIdDeclaration
    idBL, // BL
    idTRUE, // TRUE
    idFALSE, // FALSE
    idS0, // S0
    idR0, // R0
    idCS0, // CS0
    idSTATE, // STATE
    idBASE, // BASE
    idDPL, // DPL
    idTRACE, // TRACE
    idTO_IN, // >IN
    idNR_IN, // #IN
    idBLK, // BLK
    idSOURCE_ID, // SOURCE_ID
    idPAD, // PAD
    idPLUS, // +
    idMULT, // *
    idMINUS, // -
    idDIV, // /
    idMOD, // MOD
    idDIV_MOD, // /MOD
    idMULT_DIV, // */
    idMULT_DIV_MOD, // */MOD
    idFM_DIV_MOD, // FM/MOD
    idUM_DIV_MOD, // UM/MOD
    idSM_DIV_REM, // SM/REM
    idM_STAR, // M*
    idONE_PLUS, // 1+
    idONE_MINUS, // 1-
    idTWO_MULT, // 2*
    idTWO_DIV, // 2/
    idNEGATE, // NEGATE
    idS_TO_D, // S>D
    idUM_MULT, // UM*
    idABS, // ABS
    idFMAX, // MAX
    idFMIN, // MIN
    idCHAR_PLUS, // CHAR+
    idCHARS, // CHARS
    idCELL_PLUS, // CELL+
    idCELLS, // CELLS
    idWITHIN, // WITHIN
    idAND, // AND
    idOR, // OR
    idXOR, // XOR
    idINVERT, // INVERT
    idLSHIFT, // LSHIFT
    idRSHIFT, // RSHIFT
    idEQUAL, // =
    idDIFFERENT, // <>
    idLESS, // <
    idLESS_EQUAL, // <=
    idGREATER, // >
    idGREATER_EQUAL, // >=
    idU_LESS, // U<
    idU_LESS_EQUAL, // U<=
    idU_GREATER, // U>
    idU_GREATER_EQUAL, // U>=
    idZERO_EQUAL, // 0=
    idZERO_DIFFERENT, // 0<>
    idZERO_LESS, // 0<
    idZERO_LESS_EQUAL, // 0<=
    idZERO_GREATER, // 0>
    idZERO_GREATER_EQUAL, // 0>=
    idSTORE, // !
    idFETCH, // @
    idPLUS_STORE, // +!
    idCSTORE, // C!
    idCFETCH, // C@
    idTWO_STORE, // 2!
    idTWO_FETCH, // 2@
    idFILL, // FILL
    idERASE, // ERASE
    idMOVE, // MOVE
    idDROP, // DROP
    idSWAP, // SWAP
    idDUP, // DUP
    idQ_DUP, // ?DUP
    idOVER, // OVER
    idROT, // ROT
    idMINUS_ROT, // -ROT
    idNIP, // NIP
    idPICK, // PICK
    idROLL, // ROLL
    idTUCK, // TUCK
    idTWO_DROP, // 2DROP
    idTWO_SWAP, // 2SWAP
    idTWO_DUP, // 2DUP
    idTWO_OVER, // 2OVER
    idTWO_ROT, // 2ROT
    idMINUS_2ROT, // -2ROT
    idDEPTH, // DEPTH
    idSP_FETCH, // SP@
    idSP_STORE, // SP!
    idDOT_S, // .S
    idTO_R, // >R
    idFROM_R, // R>
    idR_DROP, // RDROP
    idR_FETCH, // R@
    idI, // I
    idJ, // J
    idTWO_TO_R, // 2>R
    idTWO_R_TO, // 2R>
    idTWO_R_FETCH, // 2R@
    idR_DEPTH, // RDEPTH
    idRSP_FETCH, // RSP@
    idRSP_STORE, // RSP!
    idDOT_RS, // .RS
    idCOMMA, // ,
    idCCOMMA, // C,
    idHERE, // HERE
    idLATEST, // LATEST
    idFIND, // FIND
    idTO_BODY, // >BODY
    idALIGN, // ALIGN
    idALIGNED, // ALIGNED
    idALLOT, // ALLOT
    idUNUSED, // UNUSED
    idMARKER, // MARKER
    idXMARKER, // (MARKER)
    idTYPE, // TYPE
    idEMIT, // EMIT
    idCR, // CR
    idSPACE, // SPACE
    idSPACES, // SPACES
    idLESS_HASH, // <#
    idHASH, // #
    idHASH_S, // #S
    idHOLD, // HOLD
    idSIGN, // SIGN
    idHASH_GREATER, // #>
    idDOT, // .
    idD_DOT, // D.
    idD_DOT_R, // D.R
    idU_DOT, // U.
    idDOT_R, // .R
    idU_DOT_R, // U.R
    idRDEPTH, // RDEPTH
    idCS_DEPTH, // CS_DEPTH
    idTHROW, // THROW
    idENVIRONMENT_Q, // ENVIRONMENT?
    idCOUNT, // COUNT
    idS_QUOTE, // S"
    idWORD, // WORD
    idWORDS, // WORDS
    idDABS, // DABS
    idDECIMAL, // DECIMAL
    idHEX, // HEX
    idBYE, // BYE
    idXDOVAR, // (DOVAR)
    idLITERAL, // LITERAL
    idXLITERAL, // (LITERAL)
    idINTERPRET, // INTERPRET
    idQUIT, // QUIT
    //@@END
    MAX_WORD_ID
};

//@@BEGIN: WordsXtDeclaration
extern int xtBL; // BL
extern int xtTRUE; // TRUE
extern int xtFALSE; // FALSE
extern int xtS0; // S0
extern int xtR0; // R0
extern int xtCS0; // CS0
extern int xtSTATE; // STATE
extern int xtBASE; // BASE
extern int xtDPL; // DPL
extern int xtTRACE; // TRACE
extern int xtTO_IN; // >IN
extern int xtNR_IN; // #IN
extern int xtBLK; // BLK
extern int xtSOURCE_ID; // SOURCE_ID
extern int xtPAD; // PAD
extern int xtPLUS; // +
extern int xtMULT; // *
extern int xtMINUS; // -
extern int xtDIV; // /
extern int xtMOD; // MOD
extern int xtDIV_MOD; // /MOD
extern int xtMULT_DIV; // */
extern int xtMULT_DIV_MOD; // */MOD
extern int xtFM_DIV_MOD; // FM/MOD
extern int xtUM_DIV_MOD; // UM/MOD
extern int xtSM_DIV_REM; // SM/REM
extern int xtM_STAR; // M*
extern int xtONE_PLUS; // 1+
extern int xtONE_MINUS; // 1-
extern int xtTWO_MULT; // 2*
extern int xtTWO_DIV; // 2/
extern int xtNEGATE; // NEGATE
extern int xtS_TO_D; // S>D
extern int xtUM_MULT; // UM*
extern int xtABS; // ABS
extern int xtFMAX; // MAX
extern int xtFMIN; // MIN
extern int xtCHAR_PLUS; // CHAR+
extern int xtCHARS; // CHARS
extern int xtCELL_PLUS; // CELL+
extern int xtCELLS; // CELLS
extern int xtWITHIN; // WITHIN
extern int xtAND; // AND
extern int xtOR; // OR
extern int xtXOR; // XOR
extern int xtINVERT; // INVERT
extern int xtLSHIFT; // LSHIFT
extern int xtRSHIFT; // RSHIFT
extern int xtEQUAL; // =
extern int xtDIFFERENT; // <>
extern int xtLESS; // <
extern int xtLESS_EQUAL; // <=
extern int xtGREATER; // >
extern int xtGREATER_EQUAL; // >=
extern int xtU_LESS; // U<
extern int xtU_LESS_EQUAL; // U<=
extern int xtU_GREATER; // U>
extern int xtU_GREATER_EQUAL; // U>=
extern int xtZERO_EQUAL; // 0=
extern int xtZERO_DIFFERENT; // 0<>
extern int xtZERO_LESS; // 0<
extern int xtZERO_LESS_EQUAL; // 0<=
extern int xtZERO_GREATER; // 0>
extern int xtZERO_GREATER_EQUAL; // 0>=
extern int xtSTORE; // !
extern int xtFETCH; // @
extern int xtPLUS_STORE; // +!
extern int xtCSTORE; // C!
extern int xtCFETCH; // C@
extern int xtTWO_STORE; // 2!
extern int xtTWO_FETCH; // 2@
extern int xtFILL; // FILL
extern int xtERASE; // ERASE
extern int xtMOVE; // MOVE
extern int xtDROP; // DROP
extern int xtSWAP; // SWAP
extern int xtDUP; // DUP
extern int xtQ_DUP; // ?DUP
extern int xtOVER; // OVER
extern int xtROT; // ROT
extern int xtMINUS_ROT; // -ROT
extern int xtNIP; // NIP
extern int xtPICK; // PICK
extern int xtROLL; // ROLL
extern int xtTUCK; // TUCK
extern int xtTWO_DROP; // 2DROP
extern int xtTWO_SWAP; // 2SWAP
extern int xtTWO_DUP; // 2DUP
extern int xtTWO_OVER; // 2OVER
extern int xtTWO_ROT; // 2ROT
extern int xtMINUS_2ROT; // -2ROT
extern int xtDEPTH; // DEPTH
extern int xtSP_FETCH; // SP@
extern int xtSP_STORE; // SP!
extern int xtDOT_S; // .S
extern int xtTO_R; // >R
extern int xtFROM_R; // R>
extern int xtR_DROP; // RDROP
extern int xtR_FETCH; // R@
extern int xtI; // I
extern int xtJ; // J
extern int xtTWO_TO_R; // 2>R
extern int xtTWO_R_TO; // 2R>
extern int xtTWO_R_FETCH; // 2R@
extern int xtR_DEPTH; // RDEPTH
extern int xtRSP_FETCH; // RSP@
extern int xtRSP_STORE; // RSP!
extern int xtDOT_RS; // .RS
extern int xtCOMMA; // ,
extern int xtCCOMMA; // C,
extern int xtHERE; // HERE
extern int xtLATEST; // LATEST
extern int xtFIND; // FIND
extern int xtTO_BODY; // >BODY
extern int xtALIGN; // ALIGN
extern int xtALIGNED; // ALIGNED
extern int xtALLOT; // ALLOT
extern int xtUNUSED; // UNUSED
extern int xtMARKER; // MARKER
extern int xtXMARKER; // (MARKER)
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
extern int xtRDEPTH; // RDEPTH
extern int xtCS_DEPTH; // CS_DEPTH
extern int xtTHROW; // THROW
extern int xtENVIRONMENT_Q; // ENVIRONMENT?
extern int xtCOUNT; // COUNT
extern int xtS_QUOTE; // S"
extern int xtWORD; // WORD
extern int xtWORDS; // WORDS
extern int xtDABS; // DABS
extern int xtDECIMAL; // DECIMAL
extern int xtHEX; // HEX
extern int xtBYE; // BYE
extern int xtXDOVAR; // (DOVAR)
extern int xtLITERAL; // LITERAL
extern int xtXLITERAL; // (LITERAL)
extern int xtINTERPRET; // INTERPRET
extern int xtQUIT; // QUIT
//@@END

//@@BEGIN: Constants
static inline const int cBL = BL; // BL
static inline const int cTRUE = F_TRUE; // TRUE
static inline const int cFALSE = F_FALSE; // FALSE
static inline const int cS0 = STACK_SZ; // S0
static inline const int cR0 = STACK_SZ; // R0
static inline const int cCS0 = STACK_SZ; // CS0
//@@END

// user variables
struct User {
    //@@BEGIN: Vars
    int STATE;
    int BASE;
    int DPL;
    int TRACE;
    int TO_IN;
    int NR_IN;
    int BLK;
    int SOURCE_ID;
    //@@END

    void init();
};

//@@BEGIN: WordsDeclaration
void fBL(); // BL
void fTRUE(); // TRUE
void fFALSE(); // FALSE
void fS0(); // S0
void fR0(); // R0
void fCS0(); // CS0
void fSTATE(); // STATE
void fBASE(); // BASE
void fDPL(); // DPL
void fTRACE(); // TRACE
void fTO_IN(); // >IN
void fNR_IN(); // #IN
void fBLK(); // BLK
void fSOURCE_ID(); // SOURCE_ID
void fPAD(); // PAD
void fPLUS(); // +
void fMULT(); // *
void fMINUS(); // -
void fDIV(); // /
void fMOD(); // MOD
void fDIV_MOD(); // /MOD
void fMULT_DIV(); // */
void fMULT_DIV_MOD(); // */MOD
void fFM_DIV_MOD(); // FM/MOD
void fUM_DIV_MOD(); // UM/MOD
void fSM_DIV_REM(); // SM/REM
void fM_STAR(); // M*
void fONE_PLUS(); // 1+
void fONE_MINUS(); // 1-
void fTWO_MULT(); // 2*
void fTWO_DIV(); // 2/
void fNEGATE(); // NEGATE
void fS_TO_D(); // S>D
void fUM_MULT(); // UM*
void fABS(); // ABS
void fFMAX(); // MAX
void fFMIN(); // MIN
void fCHAR_PLUS(); // CHAR+
void fCHARS(); // CHARS
void fCELL_PLUS(); // CELL+
void fCELLS(); // CELLS
void fWITHIN(); // WITHIN
void fAND(); // AND
void fOR(); // OR
void fXOR(); // XOR
void fINVERT(); // INVERT
void fLSHIFT(); // LSHIFT
void fRSHIFT(); // RSHIFT
void fEQUAL(); // =
void fDIFFERENT(); // <>
void fLESS(); // <
void fLESS_EQUAL(); // <=
void fGREATER(); // >
void fGREATER_EQUAL(); // >=
void fU_LESS(); // U<
void fU_LESS_EQUAL(); // U<=
void fU_GREATER(); // U>
void fU_GREATER_EQUAL(); // U>=
void fZERO_EQUAL(); // 0=
void fZERO_DIFFERENT(); // 0<>
void fZERO_LESS(); // 0<
void fZERO_LESS_EQUAL(); // 0<=
void fZERO_GREATER(); // 0>
void fZERO_GREATER_EQUAL(); // 0>=
void fSTORE(); // !
void fFETCH(); // @
void fPLUS_STORE(); // +!
void fCSTORE(); // C!
void fCFETCH(); // C@
void fTWO_STORE(); // 2!
void fTWO_FETCH(); // 2@
void fFILL(); // FILL
void fERASE(); // ERASE
void fMOVE(); // MOVE
void fDROP(); // DROP
void fSWAP(); // SWAP
void fDUP(); // DUP
void fQ_DUP(); // ?DUP
void fOVER(); // OVER
void fROT(); // ROT
void fMINUS_ROT(); // -ROT
void fNIP(); // NIP
void fPICK(); // PICK
void fROLL(); // ROLL
void fTUCK(); // TUCK
void fTWO_DROP(); // 2DROP
void fTWO_SWAP(); // 2SWAP
void fTWO_DUP(); // 2DUP
void fTWO_OVER(); // 2OVER
void fTWO_ROT(); // 2ROT
void fMINUS_2ROT(); // -2ROT
void fDEPTH(); // DEPTH
void fSP_FETCH(); // SP@
void fSP_STORE(); // SP!
void fDOT_S(); // .S
void fTO_R(); // >R
void fFROM_R(); // R>
void fR_DROP(); // RDROP
void fR_FETCH(); // R@
void fI(); // I
void fJ(); // J
void fTWO_TO_R(); // 2>R
void fTWO_R_TO(); // 2R>
void fTWO_R_FETCH(); // 2R@
void fR_DEPTH(); // RDEPTH
void fRSP_FETCH(); // RSP@
void fRSP_STORE(); // RSP!
void fDOT_RS(); // .RS
void fCOMMA(); // ,
void fCCOMMA(); // C,
void fHERE(); // HERE
void fLATEST(); // LATEST
void fFIND(); // FIND
void fTO_BODY(); // >BODY
void fALIGN(); // ALIGN
void fALIGNED(); // ALIGNED
void fALLOT(); // ALLOT
void fUNUSED(); // UNUSED
void fMARKER(); // MARKER
void fXMARKER(); // (MARKER)
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
void fRDEPTH(); // RDEPTH
void fCS_DEPTH(); // CS_DEPTH
void fTHROW(); // THROW
void fENVIRONMENT_Q(); // ENVIRONMENT?
void fCOUNT(); // COUNT
void fS_QUOTE(); // S"
void fWORD(); // WORD
void fWORDS(); // WORDS
void fDABS(); // DABS
void fDECIMAL(); // DECIMAL
void fHEX(); // HEX
void fBYE(); // BYE
void fXDOVAR(); // (DOVAR)
void fLITERAL(); // LITERAL
void fXLITERAL(); // (LITERAL)
void fINTERPRET(); // INTERPRET
void fQUIT(); // QUIT
//@@END

// bool
int f_bool(bool f);

// alignment and double cells
int aligned(int x);
int dcell_lo(dint x);
int dcell_hi(dint x);
dint dcell(int hi, int lo);

// pointer - address conversion
int mem_addr(const char* ptr);
int mem_addr(const int* ptr);
char* mem_char_ptr(int addr, int size = 0);
int* mem_int_ptr(int addr, int size = 0);

// access memory
int fetch(int addr);
void store(int addr, int value);
dint dfetch(int addr);
void dstore(int addr, dint value);
int cfetch(int addr);
void cstore(int addr, int value);

// allot dictionary space
void ccomma(int value);
void comma(int value);
void dcomma(dint value);
void align();

// stacks
void push(int value);
int pop();
int peek(int depth = 0);
int depth();
void roll(int depth);

void dpush(dint value);
dint dpop();
dint dpeek(int depth = 0);

void cs_push(int value);
int cs_pop();
int cs_peek(int depth = 0);
int cs_depth();

void r_push(int value);
int r_pop();
int r_peek(int depth = 0);
int r_depth();

void r_dpush(dint value);
dint r_dpop();
dint r_dpeek(int depth = 0);

// create dictionary entries
void create_dictionary();

// execute a word given its xt
void f_execute(int xt);
