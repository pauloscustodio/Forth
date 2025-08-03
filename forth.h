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
static inline const int WORDBUF_SZ = 2 * BUFFER_SZ;
static inline const int PAD_SZ = 256;
static inline const int STACK_SZ = 1024 * CELL_SZ;
static inline const int MAX_FILES = 16;
static inline const int MAX_COUNTED_STRING_SZ = 0xff;
static inline const int MAX_STRING_SZ = BUFFER_SZ;

// constants
static inline const char BL = ' ';
static inline const char CR = '\n';
static inline const int SCREEN_WIDTH = 80;

static inline const int BLOCK_ROWS = 16;
static inline const int BLOCK_COLS = 64;

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
    idBL, // BL .
    idS0, // S0 .
    idR0, // R0 .
    idCS0, // CS0 .
    idTRUE, // TRUE .
    idFALSE, // FALSE .
    idSPAN, // SPAN .
    idSTATE, // STATE .
    idDPL, // DPL .
    idTRACE, // TRACE .
    idBASE, // BASE .
    idPAD, // PAD .
    idDECIMAL, // DECIMAL .
    idHEX, // HEX .
    idPLUS, // + .
    idMULT, // * .
    idMINUS, // - .
    idDIV, // / .
    idMOD, // MOD .
    idDIV_MOD, // /MOD .
    idMULT_DIV, // */ .
    idMULT_DIV_MOD, // */MOD .
    idFM_DIV_MOD, // FM/MOD .
    idUM_DIV_MOD, // UM/MOD .
    idSM_DIV_REM, // SM/REM .
    idM_STAR, // M* .
    idONE_PLUS, // 1+ .
    idONE_MINUS, // 1- .
    idTWO_MULT, // 2* .
    idTWO_DIV, // 2/ .
    idNEGATE, // NEGATE .
    idS_TO_D, // S>D .
    idUM_MULT, // UM* .
    idABS, // ABS .
    idFMAX, // MAX .
    idFMIN, // MIN .
    idCHAR_PLUS, // CHAR+ .
    idCHARS, // CHARS .
    idCELL_PLUS, // CELL+ .
    idCELLS, // CELLS .
    idWITHIN, // WITHIN .
    idAND, // AND .
    idOR, // OR .
    idXOR, // XOR .
    idINVERT, // INVERT .
    idLSHIFT, // LSHIFT .
    idRSHIFT, // RSHIFT .
    idEQUAL, // = .
    idDIFFERENT, // <> .
    idLESS, // < .
    idLESS_EQUAL, // <= .
    idGREATER, // > .
    idGREATER_EQUAL, // >= .
    idU_LESS, // U< .
    idU_LESS_EQUAL, // U<= .
    idU_GREATER, // U> .
    idU_GREATER_EQUAL, // U>= .
    idZERO_EQUAL, // 0= .
    idZERO_DIFFERENT, // 0<> .
    idZERO_LESS, // 0< .
    idZERO_LESS_EQUAL, // 0<= .
    idZERO_GREATER, // 0> .
    idZERO_GREATER_EQUAL, // 0>= .
    idSTORE, // ! .
    idFETCH, // @ .
    idPLUS_STORE, // +! .
    idCSTORE, // C! .
    idCFETCH, // C@ .
    idTWO_STORE, // 2! .
    idTWO_FETCH, // 2@ .
    idFILL, // FILL .
    idERASE, // ERASE .
    idMOVE, // MOVE .
    idDROP, // DROP .
    idSWAP, // SWAP .
    idDUP, // DUP .
    idQ_DUP, // ?DUP .
    idOVER, // OVER .
    idROT, // ROT .
    idMINUS_ROT, // -ROT .
    idDEPTH, // DEPTH .
    idNIP, // NIP .
    idPICK, // PICK .
    idROLL, // ROLL .
    idTUCK, // TUCK .
    idTWO_DROP, // 2DROP .
    idTWO_SWAP, // 2SWAP .
    idTWO_DUP, // 2DUP .
    idTWO_OVER, // 2OVER .
    idTWO_ROT, // 2ROT .
    idMINUS_2ROT, // -2ROT .
    idSP_FETCH, // SP@ .
    idSP_STORE, // SP! .
    idTO_R, // >R .
    idFROM_R, // R> .
    idR_DROP, // RDROP .
    idRSP_FETCH, // RSP@ .
    idRSP_STORE, // RSP! .
    idR_FETCH, // R@ .
    idI, // I .
    idJ, // J .
    idTWO_TO_R, // 2>R .
    idTWO_R_TO, // 2R> .
    idTWO_R_FETCH, // 2R@ .
    idCOMMA, // , .
    idCCOMMA, // C, .
    idHERE, // HERE .
    idLATEST, // LATEST .
    idFIND, // FIND .
    idTO_BODY, // >BODY .
    idALIGN, // ALIGN .
    idALIGNED, // ALIGNED .
    idALLOT, // ALLOT .
    idUNUSED, // UNUSED .
    idMARKER, // MARKER .
    idXMARKER, // (MARKER) .
    idPARSE, // PARSE .
    idPARSE_WORD, // PARSE-WORD .
    idWORD, // WORD .
    idCHAR, // CHAR .
    idBRACKET_CHAR, // [CHAR] .
    idNUMBER_Q, // NUMBER? .
    idNUMBER, // NUMBER .
    idTO_NUMBER, // >NUMBER .
    idCONVERT, // CONVERT .
    idCOUNT, // COUNT .
    idDOT_QUOTE, // ." .
    idXDOT_QUOTE, // (.") .
    idS_QUOTE, // S" .
    idXS_QUOTE, // (S") .
    idC_QUOTE, // C" .
    idXC_QUOTE, // (C") .
    idDOT_PAREN, // .( .
    idTIB, // TIB .
    idNR_IN, // #IN .
    idNR_TIB, // #TIB .
    idTO_IN, // >IN .
    idSOURCE, // SOURCE .
    idSOURCE_ID, // SOURCE-ID .
    idREFILL, // REFILL .
    idACCEPT, // ACCEPT .
    idEXPECT, // EXPECT .
    idKEY, // KEY .
    idQUERY, // QUERY .
    idSAVE_INPUT, // SAVE-INPUT .
    idRESTORE_INPUT, // RESTORE-INPUT .
    idTYPE, // TYPE .
    idEMIT, // EMIT .
    idCR, // CR .
    idSPACE, // SPACE .
    idSPACES, // SPACES .
    idLESS_HASH, // <# .
    idHASH, // # .
    idHASH_S, // #S .
    idHOLD, // HOLD .
    idSIGN, // SIGN .
    idHASH_GREATER, // #> .
    idDOT, // . .
    idD_DOT, // D. .
    idD_DOT_R, // D.R .
    idU_DOT, // U. .
    idDOT_R, // .R .
    idU_DOT_R, // U.R .
    idINTERPRET, // INTERPRET .
    idEVALUATE, // EVALUATE .
    idEXECUTE, // EXECUTE .
    idEXIT, // EXIT .
    idCOLON, // : .
    idCOLON_NONAME, // :NONAME .
    idSEMICOLON, // ; .
    idXDOCOL, // (DOCOL) .
    idCREATE, // CREATE .
    idVARIABLE, // VARIABLE .
    idXDOVAR, // (DOVAR) .
    idTWO_VARIABLE, // 2VARIABLE .
    idVALUE, // VALUE .
    idTO, // TO .
    idCONSTANT, // CONSTANT .
    idXDOCONST, // (DOCONST) .
    idTWO_CONSTANT, // 2CONSTANT .
    idXDO2CONST, // (DO2CONST) .
    idLITERAL, // LITERAL .
    idXLITERAL, // (LITERAL) .
    idTWO_LITERAL, // 2LITERAL .
    idX2LITERAL, // (2LITERAL) .
    idDOES, // DOES> .
    idXDOES_DEFINE, // (DOES>DEFINE) .
    idXDOES_RUN, // (DOES>RUN) .
    idLBRACKET, // [ .
    idRBRACKET, // ] .
    idOPEN_PAREN, // ( .
    idBACKSLASH, // \ .
    idBRANCH, // BRANCH .
    idZBRANCH, // 0BRANCH .
    idTICK, // ' .
    idBRACKET_TICK, // ['] .
    idPOSTPONE, // POSTPONE .
    idIMMEDIATE, // IMMEDIATE .
    idCOMPILE_COMMA, // COMPILE, .
    idBRACKET_COMPILE, // [COMPILE] .
    idIF, // IF .
    idELSE, // ELSE .
    idTHEN, // THEN .
    idDO, // DO .
    idXDO, // (DO) .
    idQUERY_DO, // ?DO .
    idXQUERY_DO, // (?DO) .
    idLOOP, // LOOP .
    idXLOOP, // (LOOP) .
    idPLUS_LOOP, // +LOOP .
    idXPLUS_LOOP, // (+LOOP) .
    idLEAVE, // LEAVE .
    idXLEAVE, // (LEAVE) .
    idUNLOOP, // UNLOOP .
    idXUNLOOP, // (UNLOOP) .
    idBEGIN, // BEGIN .
    idAGAIN, // AGAIN .
    idUNTIL, // UNTIL .
    idWHILE, // WHILE .
    idREPEAT, // REPEAT .
    idRECURSE, // RECURSE .
    idCASE, // CASE .
    idOF, // OF .
    idXOF, // (OF) .
    idENDOF, // ENDOF .
    idENDCASE, // ENDCASE .
    idQUIT, // QUIT .
    idBYE, // BYE .
    idABORT, // ABORT .
    idABORT_QUOTE, // ABORT" .
    idXABORT_QUOTE, // (ABORT") .
    idDOT_S, // .S .
    idDOT_RS, // .RS .
    idWORDS, // WORDS .
    idENVIRONMENT_Q, // ENVIRONMENT? .
    idBLK, // BLK .
    idLOAD, // LOAD .
    idTHROW, // THROW .
    idDABS, // DABS .
    //@@END
    MAX_WORD_ID
};

//@@BEGIN: WordsXtDeclaration
extern int xtBL; // BL .
extern int xtS0; // S0 .
extern int xtR0; // R0 .
extern int xtCS0; // CS0 .
extern int xtTRUE; // TRUE .
extern int xtFALSE; // FALSE .
extern int xtSPAN; // SPAN .
extern int xtSTATE; // STATE .
extern int xtDPL; // DPL .
extern int xtTRACE; // TRACE .
extern int xtBASE; // BASE .
extern int xtPAD; // PAD .
extern int xtDECIMAL; // DECIMAL .
extern int xtHEX; // HEX .
extern int xtPLUS; // + .
extern int xtMULT; // * .
extern int xtMINUS; // - .
extern int xtDIV; // / .
extern int xtMOD; // MOD .
extern int xtDIV_MOD; // /MOD .
extern int xtMULT_DIV; // */ .
extern int xtMULT_DIV_MOD; // */MOD .
extern int xtFM_DIV_MOD; // FM/MOD .
extern int xtUM_DIV_MOD; // UM/MOD .
extern int xtSM_DIV_REM; // SM/REM .
extern int xtM_STAR; // M* .
extern int xtONE_PLUS; // 1+ .
extern int xtONE_MINUS; // 1- .
extern int xtTWO_MULT; // 2* .
extern int xtTWO_DIV; // 2/ .
extern int xtNEGATE; // NEGATE .
extern int xtS_TO_D; // S>D .
extern int xtUM_MULT; // UM* .
extern int xtABS; // ABS .
extern int xtFMAX; // MAX .
extern int xtFMIN; // MIN .
extern int xtCHAR_PLUS; // CHAR+ .
extern int xtCHARS; // CHARS .
extern int xtCELL_PLUS; // CELL+ .
extern int xtCELLS; // CELLS .
extern int xtWITHIN; // WITHIN .
extern int xtAND; // AND .
extern int xtOR; // OR .
extern int xtXOR; // XOR .
extern int xtINVERT; // INVERT .
extern int xtLSHIFT; // LSHIFT .
extern int xtRSHIFT; // RSHIFT .
extern int xtEQUAL; // = .
extern int xtDIFFERENT; // <> .
extern int xtLESS; // < .
extern int xtLESS_EQUAL; // <= .
extern int xtGREATER; // > .
extern int xtGREATER_EQUAL; // >= .
extern int xtU_LESS; // U< .
extern int xtU_LESS_EQUAL; // U<= .
extern int xtU_GREATER; // U> .
extern int xtU_GREATER_EQUAL; // U>= .
extern int xtZERO_EQUAL; // 0= .
extern int xtZERO_DIFFERENT; // 0<> .
extern int xtZERO_LESS; // 0< .
extern int xtZERO_LESS_EQUAL; // 0<= .
extern int xtZERO_GREATER; // 0> .
extern int xtZERO_GREATER_EQUAL; // 0>= .
extern int xtSTORE; // ! .
extern int xtFETCH; // @ .
extern int xtPLUS_STORE; // +! .
extern int xtCSTORE; // C! .
extern int xtCFETCH; // C@ .
extern int xtTWO_STORE; // 2! .
extern int xtTWO_FETCH; // 2@ .
extern int xtFILL; // FILL .
extern int xtERASE; // ERASE .
extern int xtMOVE; // MOVE .
extern int xtDROP; // DROP .
extern int xtSWAP; // SWAP .
extern int xtDUP; // DUP .
extern int xtQ_DUP; // ?DUP .
extern int xtOVER; // OVER .
extern int xtROT; // ROT .
extern int xtMINUS_ROT; // -ROT .
extern int xtDEPTH; // DEPTH .
extern int xtNIP; // NIP .
extern int xtPICK; // PICK .
extern int xtROLL; // ROLL .
extern int xtTUCK; // TUCK .
extern int xtTWO_DROP; // 2DROP .
extern int xtTWO_SWAP; // 2SWAP .
extern int xtTWO_DUP; // 2DUP .
extern int xtTWO_OVER; // 2OVER .
extern int xtTWO_ROT; // 2ROT .
extern int xtMINUS_2ROT; // -2ROT .
extern int xtSP_FETCH; // SP@ .
extern int xtSP_STORE; // SP! .
extern int xtTO_R; // >R .
extern int xtFROM_R; // R> .
extern int xtR_DROP; // RDROP .
extern int xtRSP_FETCH; // RSP@ .
extern int xtRSP_STORE; // RSP! .
extern int xtR_FETCH; // R@ .
extern int xtI; // I .
extern int xtJ; // J .
extern int xtTWO_TO_R; // 2>R .
extern int xtTWO_R_TO; // 2R> .
extern int xtTWO_R_FETCH; // 2R@ .
extern int xtCOMMA; // , .
extern int xtCCOMMA; // C, .
extern int xtHERE; // HERE .
extern int xtLATEST; // LATEST .
extern int xtFIND; // FIND .
extern int xtTO_BODY; // >BODY .
extern int xtALIGN; // ALIGN .
extern int xtALIGNED; // ALIGNED .
extern int xtALLOT; // ALLOT .
extern int xtUNUSED; // UNUSED .
extern int xtMARKER; // MARKER .
extern int xtXMARKER; // (MARKER) .
extern int xtPARSE; // PARSE .
extern int xtPARSE_WORD; // PARSE-WORD .
extern int xtWORD; // WORD .
extern int xtCHAR; // CHAR .
extern int xtBRACKET_CHAR; // [CHAR] .
extern int xtNUMBER_Q; // NUMBER? .
extern int xtNUMBER; // NUMBER .
extern int xtTO_NUMBER; // >NUMBER .
extern int xtCONVERT; // CONVERT .
extern int xtCOUNT; // COUNT .
extern int xtDOT_QUOTE; // ." .
extern int xtXDOT_QUOTE; // (.") .
extern int xtS_QUOTE; // S" .
extern int xtXS_QUOTE; // (S") .
extern int xtC_QUOTE; // C" .
extern int xtXC_QUOTE; // (C") .
extern int xtDOT_PAREN; // .( .
extern int xtTIB; // TIB .
extern int xtNR_IN; // #IN .
extern int xtNR_TIB; // #TIB .
extern int xtTO_IN; // >IN .
extern int xtSOURCE; // SOURCE .
extern int xtSOURCE_ID; // SOURCE-ID .
extern int xtREFILL; // REFILL .
extern int xtACCEPT; // ACCEPT .
extern int xtEXPECT; // EXPECT .
extern int xtKEY; // KEY .
extern int xtQUERY; // QUERY .
extern int xtSAVE_INPUT; // SAVE-INPUT .
extern int xtRESTORE_INPUT; // RESTORE-INPUT .
extern int xtTYPE; // TYPE .
extern int xtEMIT; // EMIT .
extern int xtCR; // CR .
extern int xtSPACE; // SPACE .
extern int xtSPACES; // SPACES .
extern int xtLESS_HASH; // <# .
extern int xtHASH; // # .
extern int xtHASH_S; // #S .
extern int xtHOLD; // HOLD .
extern int xtSIGN; // SIGN .
extern int xtHASH_GREATER; // #> .
extern int xtDOT; // . .
extern int xtD_DOT; // D. .
extern int xtD_DOT_R; // D.R .
extern int xtU_DOT; // U. .
extern int xtDOT_R; // .R .
extern int xtU_DOT_R; // U.R .
extern int xtINTERPRET; // INTERPRET .
extern int xtEVALUATE; // EVALUATE .
extern int xtEXECUTE; // EXECUTE .
extern int xtEXIT; // EXIT .
extern int xtCOLON; // : .
extern int xtCOLON_NONAME; // :NONAME .
extern int xtSEMICOLON; // ; .
extern int xtXDOCOL; // (DOCOL) .
extern int xtCREATE; // CREATE .
extern int xtVARIABLE; // VARIABLE .
extern int xtXDOVAR; // (DOVAR) .
extern int xtTWO_VARIABLE; // 2VARIABLE .
extern int xtVALUE; // VALUE .
extern int xtTO; // TO .
extern int xtCONSTANT; // CONSTANT .
extern int xtXDOCONST; // (DOCONST) .
extern int xtTWO_CONSTANT; // 2CONSTANT .
extern int xtXDO2CONST; // (DO2CONST) .
extern int xtLITERAL; // LITERAL .
extern int xtXLITERAL; // (LITERAL) .
extern int xtTWO_LITERAL; // 2LITERAL .
extern int xtX2LITERAL; // (2LITERAL) .
extern int xtDOES; // DOES> .
extern int xtXDOES_DEFINE; // (DOES>DEFINE) .
extern int xtXDOES_RUN; // (DOES>RUN) .
extern int xtLBRACKET; // [ .
extern int xtRBRACKET; // ] .
extern int xtOPEN_PAREN; // ( .
extern int xtBACKSLASH; // \ .
extern int xtBRANCH; // BRANCH .
extern int xtZBRANCH; // 0BRANCH .
extern int xtTICK; // ' .
extern int xtBRACKET_TICK; // ['] .
extern int xtPOSTPONE; // POSTPONE .
extern int xtIMMEDIATE; // IMMEDIATE .
extern int xtCOMPILE_COMMA; // COMPILE, .
extern int xtBRACKET_COMPILE; // [COMPILE] .
extern int xtIF; // IF .
extern int xtELSE; // ELSE .
extern int xtTHEN; // THEN .
extern int xtDO; // DO .
extern int xtXDO; // (DO) .
extern int xtQUERY_DO; // ?DO .
extern int xtXQUERY_DO; // (?DO) .
extern int xtLOOP; // LOOP .
extern int xtXLOOP; // (LOOP) .
extern int xtPLUS_LOOP; // +LOOP .
extern int xtXPLUS_LOOP; // (+LOOP) .
extern int xtLEAVE; // LEAVE .
extern int xtXLEAVE; // (LEAVE) .
extern int xtUNLOOP; // UNLOOP .
extern int xtXUNLOOP; // (UNLOOP) .
extern int xtBEGIN; // BEGIN .
extern int xtAGAIN; // AGAIN .
extern int xtUNTIL; // UNTIL .
extern int xtWHILE; // WHILE .
extern int xtREPEAT; // REPEAT .
extern int xtRECURSE; // RECURSE .
extern int xtCASE; // CASE .
extern int xtOF; // OF .
extern int xtXOF; // (OF) .
extern int xtENDOF; // ENDOF .
extern int xtENDCASE; // ENDCASE .
extern int xtQUIT; // QUIT .
extern int xtBYE; // BYE .
extern int xtABORT; // ABORT .
extern int xtABORT_QUOTE; // ABORT" .
extern int xtXABORT_QUOTE; // (ABORT") .
extern int xtDOT_S; // .S .
extern int xtDOT_RS; // .RS .
extern int xtWORDS; // WORDS .
extern int xtENVIRONMENT_Q; // ENVIRONMENT? .
extern int xtBLK; // BLK .
extern int xtLOAD; // LOAD .
extern int xtTHROW; // THROW .
extern int xtDABS; // DABS .
//@@END

// user variables
struct User {
    //@@BEGIN: Vars
    int SPAN;
    int STATE;
    int DPL;
    int TRACE;
    int BASE;
    //@@END

    void init();
};

// interpreter pointer
extern int ip;

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
class CountedString;
int mem_addr(const CountedString* ptr);

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

// inner interpreter
void f_execute(int xt);

