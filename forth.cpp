//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "environ.h"
#include "errors.h"
#include "forth.h"
#include "interp.h"
#include "math.h"
#include "parser.h"
#include "str.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

// interpreter pointer
int ip = 0;

//@@BEGIN: WordsXtDefinition
int xtBL = 0; // BL
int xtS0 = 0; // S0
int xtR0 = 0; // R0
int xtCS0 = 0; // CS0
int xtTRUE = 0; // TRUE
int xtFALSE = 0; // FALSE
int xtSPAN = 0; // SPAN
int xtSTATE = 0; // STATE
int xtDPL = 0; // DPL
int xtTRACE = 0; // TRACE
int xtBASE = 0; // BASE
int xtPAD = 0; // PAD
int xtDECIMAL = 0; // DECIMAL
int xtHEX = 0; // HEX
int xtPLUS = 0; // +
int xtMULT = 0; // *
int xtMINUS = 0; // -
int xtDIV = 0; // /
int xtMOD = 0; // MOD
int xtDIV_MOD = 0; // /MOD
int xtMULT_DIV = 0; // */
int xtMULT_DIV_MOD = 0; // */MOD
int xtFM_DIV_MOD = 0; // FM/MOD
int xtUM_DIV_MOD = 0; // UM/MOD
int xtSM_DIV_REM = 0; // SM/REM
int xtM_STAR = 0; // M*
int xtONE_PLUS = 0; // 1+
int xtONE_MINUS = 0; // 1-
int xtTWO_MULT = 0; // 2*
int xtTWO_DIV = 0; // 2/
int xtNEGATE = 0; // NEGATE
int xtS_TO_D = 0; // S>D
int xtUM_MULT = 0; // UM*
int xtABS = 0; // ABS
int xtFMAX = 0; // MAX
int xtFMIN = 0; // MIN
int xtCHAR_PLUS = 0; // CHAR+
int xtCHARS = 0; // CHARS
int xtCELL_PLUS = 0; // CELL+
int xtCELLS = 0; // CELLS
int xtWITHIN = 0; // WITHIN
int xtAND = 0; // AND
int xtOR = 0; // OR
int xtXOR = 0; // XOR
int xtINVERT = 0; // INVERT
int xtLSHIFT = 0; // LSHIFT
int xtRSHIFT = 0; // RSHIFT
int xtEQUAL = 0; // =
int xtDIFFERENT = 0; // <>
int xtLESS = 0; // <
int xtLESS_EQUAL = 0; // <=
int xtGREATER = 0; // >
int xtGREATER_EQUAL = 0; // >=
int xtU_LESS = 0; // U<
int xtU_LESS_EQUAL = 0; // U<=
int xtU_GREATER = 0; // U>
int xtU_GREATER_EQUAL = 0; // U>=
int xtZERO_EQUAL = 0; // 0=
int xtZERO_DIFFERENT = 0; // 0<>
int xtZERO_LESS = 0; // 0<
int xtZERO_LESS_EQUAL = 0; // 0<=
int xtZERO_GREATER = 0; // 0>
int xtZERO_GREATER_EQUAL = 0; // 0>=
int xtSTORE = 0; // !
int xtFETCH = 0; // @
int xtPLUS_STORE = 0; // +!
int xtCSTORE = 0; // C!
int xtCFETCH = 0; // C@
int xtTWO_STORE = 0; // 2!
int xtTWO_FETCH = 0; // 2@
int xtFILL = 0; // FILL
int xtERASE = 0; // ERASE
int xtMOVE = 0; // MOVE
int xtDROP = 0; // DROP
int xtSWAP = 0; // SWAP
int xtDUP = 0; // DUP
int xtQ_DUP = 0; // ?DUP
int xtOVER = 0; // OVER
int xtROT = 0; // ROT
int xtMINUS_ROT = 0; // -ROT
int xtDEPTH = 0; // DEPTH
int xtNIP = 0; // NIP
int xtPICK = 0; // PICK
int xtROLL = 0; // ROLL
int xtTUCK = 0; // TUCK
int xtTWO_DROP = 0; // 2DROP
int xtTWO_SWAP = 0; // 2SWAP
int xtTWO_DUP = 0; // 2DUP
int xtTWO_OVER = 0; // 2OVER
int xtTWO_ROT = 0; // 2ROT
int xtMINUS_2ROT = 0; // -2ROT
int xtSP_FETCH = 0; // SP@
int xtSP_STORE = 0; // SP!
int xtTO_R = 0; // >R
int xtFROM_R = 0; // R>
int xtR_DROP = 0; // RDROP
int xtRSP_FETCH = 0; // RSP@
int xtRSP_STORE = 0; // RSP!
int xtR_FETCH = 0; // R@
int xtI = 0; // I
int xtJ = 0; // J
int xtTWO_TO_R = 0; // 2>R
int xtTWO_R_TO = 0; // 2R>
int xtTWO_R_FETCH = 0; // 2R@
int xtCOMMA = 0; // ,
int xtCCOMMA = 0; // C,
int xtHERE = 0; // HERE
int xtLATEST = 0; // LATEST
int xtFIND = 0; // FIND
int xtTO_BODY = 0; // >BODY
int xtALIGN = 0; // ALIGN
int xtALIGNED = 0; // ALIGNED
int xtALLOT = 0; // ALLOT
int xtUNUSED = 0; // UNUSED
int xtMARKER = 0; // MARKER
int xtXMARKER = 0; // (MARKER)
int xtPARSE = 0; // PARSE
int xtPARSE_WORD = 0; // PARSE-WORD
int xtWORD = 0; // WORD
int xtCHAR = 0; // CHAR
int xtBRACKET_CHAR = 0; // [CHAR]
int xtNUMBER_Q = 0; // NUMBER?
int xtNUMBER = 0; // NUMBER
int xtTO_NUMBER = 0; // >NUMBER
int xtCONVERT = 0; // CONVERT
int xtCOUNT = 0; // COUNT
int xtDOT_QUOTE = 0; // ."
int xtXDOT_QUOTE = 0; // (.")
int xtS_QUOTE = 0; // S"
int xtXS_QUOTE = 0; // (S")
int xtC_QUOTE = 0; // C"
int xtXC_QUOTE = 0; // (C")
int xtDOT_PAREN = 0; // .(
int xtTIB = 0; // TIB
int xtNR_IN = 0; // #IN
int xtNR_TIB = 0; // #TIB
int xtTO_IN = 0; // >IN
int xtSOURCE = 0; // SOURCE
int xtSOURCE_ID = 0; // SOURCE-ID
int xtREFILL = 0; // REFILL
int xtACCEPT = 0; // ACCEPT
int xtEXPECT = 0; // EXPECT
int xtKEY = 0; // KEY
int xtQUERY = 0; // QUERY
int xtSAVE_INPUT = 0; // SAVE-INPUT
int xtRESTORE_INPUT = 0; // RESTORE-INPUT
int xtTYPE = 0; // TYPE
int xtEMIT = 0; // EMIT
int xtCR = 0; // CR
int xtSPACE = 0; // SPACE
int xtSPACES = 0; // SPACES
int xtLESS_HASH = 0; // <#
int xtHASH = 0; // #
int xtHASH_S = 0; // #S
int xtHOLD = 0; // HOLD
int xtSIGN = 0; // SIGN
int xtHASH_GREATER = 0; // #>
int xtDOT = 0; // .
int xtD_DOT = 0; // D.
int xtD_DOT_R = 0; // D.R
int xtU_DOT = 0; // U.
int xtDOT_R = 0; // .R
int xtU_DOT_R = 0; // U.R
int xtINTERPRET = 0; // INTERPRET
int xtEVALUATE = 0; // EVALUATE
int xtEXECUTE = 0; // EXECUTE
int xtEXIT = 0; // EXIT
int xtRDEPTH = 0; // RDEPTH
int xtCS_DEPTH = 0; // CS_DEPTH
int xtDOT_S = 0; // .S
int xtR_DEPTH = 0; // RDEPTH
int xtDOT_RS = 0; // .RS
int xtBLK = 0; // BLK
int xtTHROW = 0; // THROW
int xtENVIRONMENT_Q = 0; // ENVIRONMENT?
int xtWORDS = 0; // WORDS
int xtDABS = 0; // DABS
int xtBYE = 0; // BYE
int xtXDOVAR = 0; // (DOVAR)
int xtLITERAL = 0; // LITERAL
int xtXLITERAL = 0; // (LITERAL)
int xtQUIT = 0; // QUIT
//@@END

// bool
int f_bool(bool f) {
	return f ? F_TRUE : F_FALSE;
}

// alignment and double cells
int aligned(int x) {
	return (x + CELL_SZ - 1) & ~(CELL_SZ - 1);
}

int dcell_lo(dint x) {
	return x & 0xffffffffLL;
}

int dcell_hi(dint x) {
	return (x >> 32) & 0xffffffffLL;
}

dint dcell(int hi, int lo) {
	return
		((static_cast<udint>(hi) & 0xffffffffLL) << 32) |
		(static_cast<udint>(lo) & 0xffffffffLL);
}

// pointer - address conversion
int mem_addr(const char* ptr) {
	return vm.mem.addr(ptr);
}

int mem_addr(const int* ptr) {
	return vm.mem.addr(ptr);
}

int mem_addr(const CountedString* ptr) {
	return vm.mem.addr(reinterpret_cast<const char*>(ptr));
}

char* mem_char_ptr(int addr, int size) {
	return vm.mem.char_ptr(addr, size);
}

int* mem_int_ptr(int addr, int size) {
	return vm.mem.int_ptr(addr, size);
}

// access memory
int fetch(int addr) {
	return vm.mem.fetch(addr);
}

void store(int addr, int value) {
	vm.mem.store(addr, value);
}

dint dfetch(int addr) {
	return vm.mem.dfetch(addr);
}

void dstore(int addr, dint value) {
	vm.mem.dstore(addr, value);
}

int cfetch(int addr) {
	return vm.mem.cfetch(addr);
}

void cstore(int addr, int value) {
	vm.mem.cstore(addr, value);
}

// allot dictionary space
void ccomma(int value) {
	vm.dict->ccomma(value);
}

void comma(int value) {
	vm.dict->comma(value);
}

void dcomma(dint value) {
	vm.dict->dcomma(value);
}

void align() {
	vm.dict->align();
}

// stacks
void push(int value) {
	vm.stack->push(value);
}

int pop() {
	return vm.stack->pop();
}

int peek(int depth) {
	return vm.stack->peek(depth);
}

int depth() {
	return vm.stack->depth();
}

void roll(int depth) {
	vm.stack->roll(depth);
}

void dpush(dint value) {
	vm.stack->dpush(value);
}

dint dpop() {
	return vm.stack->dpop();
}

dint dpeek(int depth) {
	return vm.stack->dpeek(depth);
}

void cs_push(int value) {
	vm.cs_stack->push(value);
}

int cs_pop() {
	return vm.cs_stack->pop();
}

int cs_peek(int depth) {
	return vm.cs_stack->peek(depth);
}

int cs_depth() {
	return vm.cs_stack->depth();
}

void r_push(int value) {
	vm.rstack->push(value);
}

int r_pop() {
	return vm.rstack->pop();
}

int r_peek(int depth) {
	return vm.rstack->peek(depth);
}

int r_depth() {
	return vm.rstack->depth();
}

void r_dpush(dint value) {
	vm.rstack->dpush(value);
}

dint r_dpop() {
	return vm.rstack->dpop();
}

dint r_dpeek(int depth) {
	return vm.rstack->dpeek(depth);
}

void create_dictionary() {
	//@@BEGIN: WordsCreateDictionary
	xtBL = vm.dict->create("BL", 0, idBL);
	xtS0 = vm.dict->create("S0", 0, idS0);
	xtR0 = vm.dict->create("R0", 0, idR0);
	xtCS0 = vm.dict->create("CS0", 0, idCS0);
	xtTRUE = vm.dict->create("TRUE", 0, idTRUE);
	xtFALSE = vm.dict->create("FALSE", 0, idFALSE);
	xtSPAN = vm.dict->create("SPAN", 0, idSPAN);
	xtSTATE = vm.dict->create("STATE", 0, idSTATE);
	xtDPL = vm.dict->create("DPL", 0, idDPL);
	xtTRACE = vm.dict->create("TRACE", 0, idTRACE);
	xtBASE = vm.dict->create("BASE", 0, idBASE);
	xtPAD = vm.dict->create("PAD", 0, idPAD);
	xtDECIMAL = vm.dict->create("DECIMAL", 0, idDECIMAL);
	xtHEX = vm.dict->create("HEX", 0, idHEX);
	xtPLUS = vm.dict->create("+", 0, idPLUS);
	xtMULT = vm.dict->create("*", 0, idMULT);
	xtMINUS = vm.dict->create("-", 0, idMINUS);
	xtDIV = vm.dict->create("/", 0, idDIV);
	xtMOD = vm.dict->create("MOD", 0, idMOD);
	xtDIV_MOD = vm.dict->create("/MOD", 0, idDIV_MOD);
	xtMULT_DIV = vm.dict->create("*/", 0, idMULT_DIV);
	xtMULT_DIV_MOD = vm.dict->create("*/MOD", 0, idMULT_DIV_MOD);
	xtFM_DIV_MOD = vm.dict->create("FM/MOD", 0, idFM_DIV_MOD);
	xtUM_DIV_MOD = vm.dict->create("UM/MOD", 0, idUM_DIV_MOD);
	xtSM_DIV_REM = vm.dict->create("SM/REM", 0, idSM_DIV_REM);
	xtM_STAR = vm.dict->create("M*", 0, idM_STAR);
	xtONE_PLUS = vm.dict->create("1+", 0, idONE_PLUS);
	xtONE_MINUS = vm.dict->create("1-", 0, idONE_MINUS);
	xtTWO_MULT = vm.dict->create("2*", 0, idTWO_MULT);
	xtTWO_DIV = vm.dict->create("2/", 0, idTWO_DIV);
	xtNEGATE = vm.dict->create("NEGATE", 0, idNEGATE);
	xtS_TO_D = vm.dict->create("S>D", 0, idS_TO_D);
	xtUM_MULT = vm.dict->create("UM*", 0, idUM_MULT);
	xtABS = vm.dict->create("ABS", 0, idABS);
	xtFMAX = vm.dict->create("MAX", 0, idFMAX);
	xtFMIN = vm.dict->create("MIN", 0, idFMIN);
	xtCHAR_PLUS = vm.dict->create("CHAR+", 0, idCHAR_PLUS);
	xtCHARS = vm.dict->create("CHARS", 0, idCHARS);
	xtCELL_PLUS = vm.dict->create("CELL+", 0, idCELL_PLUS);
	xtCELLS = vm.dict->create("CELLS", 0, idCELLS);
	xtWITHIN = vm.dict->create("WITHIN", 0, idWITHIN);
	xtAND = vm.dict->create("AND", 0, idAND);
	xtOR = vm.dict->create("OR", 0, idOR);
	xtXOR = vm.dict->create("XOR", 0, idXOR);
	xtINVERT = vm.dict->create("INVERT", 0, idINVERT);
	xtLSHIFT = vm.dict->create("LSHIFT", 0, idLSHIFT);
	xtRSHIFT = vm.dict->create("RSHIFT", 0, idRSHIFT);
	xtEQUAL = vm.dict->create("=", 0, idEQUAL);
	xtDIFFERENT = vm.dict->create("<>", 0, idDIFFERENT);
	xtLESS = vm.dict->create("<", 0, idLESS);
	xtLESS_EQUAL = vm.dict->create("<=", 0, idLESS_EQUAL);
	xtGREATER = vm.dict->create(">", 0, idGREATER);
	xtGREATER_EQUAL = vm.dict->create(">=", 0, idGREATER_EQUAL);
	xtU_LESS = vm.dict->create("U<", 0, idU_LESS);
	xtU_LESS_EQUAL = vm.dict->create("U<=", 0, idU_LESS_EQUAL);
	xtU_GREATER = vm.dict->create("U>", 0, idU_GREATER);
	xtU_GREATER_EQUAL = vm.dict->create("U>=", 0, idU_GREATER_EQUAL);
	xtZERO_EQUAL = vm.dict->create("0=", 0, idZERO_EQUAL);
	xtZERO_DIFFERENT = vm.dict->create("0<>", 0, idZERO_DIFFERENT);
	xtZERO_LESS = vm.dict->create("0<", 0, idZERO_LESS);
	xtZERO_LESS_EQUAL = vm.dict->create("0<=", 0, idZERO_LESS_EQUAL);
	xtZERO_GREATER = vm.dict->create("0>", 0, idZERO_GREATER);
	xtZERO_GREATER_EQUAL = vm.dict->create("0>=", 0, idZERO_GREATER_EQUAL);
	xtSTORE = vm.dict->create("!", 0, idSTORE);
	xtFETCH = vm.dict->create("@", 0, idFETCH);
	xtPLUS_STORE = vm.dict->create("+!", 0, idPLUS_STORE);
	xtCSTORE = vm.dict->create("C!", 0, idCSTORE);
	xtCFETCH = vm.dict->create("C@", 0, idCFETCH);
	xtTWO_STORE = vm.dict->create("2!", 0, idTWO_STORE);
	xtTWO_FETCH = vm.dict->create("2@", 0, idTWO_FETCH);
	xtFILL = vm.dict->create("FILL", 0, idFILL);
	xtERASE = vm.dict->create("ERASE", 0, idERASE);
	xtMOVE = vm.dict->create("MOVE", 0, idMOVE);
	xtDROP = vm.dict->create("DROP", 0, idDROP);
	xtSWAP = vm.dict->create("SWAP", 0, idSWAP);
	xtDUP = vm.dict->create("DUP", 0, idDUP);
	xtQ_DUP = vm.dict->create("?DUP", 0, idQ_DUP);
	xtOVER = vm.dict->create("OVER", 0, idOVER);
	xtROT = vm.dict->create("ROT", 0, idROT);
	xtMINUS_ROT = vm.dict->create("-ROT", 0, idMINUS_ROT);
	xtDEPTH = vm.dict->create("DEPTH", 0, idDEPTH);
	xtNIP = vm.dict->create("NIP", 0, idNIP);
	xtPICK = vm.dict->create("PICK", 0, idPICK);
	xtROLL = vm.dict->create("ROLL", 0, idROLL);
	xtTUCK = vm.dict->create("TUCK", 0, idTUCK);
	xtTWO_DROP = vm.dict->create("2DROP", 0, idTWO_DROP);
	xtTWO_SWAP = vm.dict->create("2SWAP", 0, idTWO_SWAP);
	xtTWO_DUP = vm.dict->create("2DUP", 0, idTWO_DUP);
	xtTWO_OVER = vm.dict->create("2OVER", 0, idTWO_OVER);
	xtTWO_ROT = vm.dict->create("2ROT", 0, idTWO_ROT);
	xtMINUS_2ROT = vm.dict->create("-2ROT", 0, idMINUS_2ROT);
	xtSP_FETCH = vm.dict->create("SP@", 0, idSP_FETCH);
	xtSP_STORE = vm.dict->create("SP!", 0, idSP_STORE);
	xtTO_R = vm.dict->create(">R", 0, idTO_R);
	xtFROM_R = vm.dict->create("R>", 0, idFROM_R);
	xtR_DROP = vm.dict->create("RDROP", 0, idR_DROP);
	xtRSP_FETCH = vm.dict->create("RSP@", 0, idRSP_FETCH);
	xtRSP_STORE = vm.dict->create("RSP!", 0, idRSP_STORE);
	xtR_FETCH = vm.dict->create("R@", 0, idR_FETCH);
	xtI = vm.dict->create("I", 0, idI);
	xtJ = vm.dict->create("J", 0, idJ);
	xtTWO_TO_R = vm.dict->create("2>R", 0, idTWO_TO_R);
	xtTWO_R_TO = vm.dict->create("2R>", 0, idTWO_R_TO);
	xtTWO_R_FETCH = vm.dict->create("2R@", 0, idTWO_R_FETCH);
	xtCOMMA = vm.dict->create(",", 0, idCOMMA);
	xtCCOMMA = vm.dict->create("C,", 0, idCCOMMA);
	xtHERE = vm.dict->create("HERE", 0, idHERE);
	xtLATEST = vm.dict->create("LATEST", 0, idLATEST);
	xtFIND = vm.dict->create("FIND", 0, idFIND);
	xtTO_BODY = vm.dict->create(">BODY", 0, idTO_BODY);
	xtALIGN = vm.dict->create("ALIGN", 0, idALIGN);
	xtALIGNED = vm.dict->create("ALIGNED", 0, idALIGNED);
	xtALLOT = vm.dict->create("ALLOT", 0, idALLOT);
	xtUNUSED = vm.dict->create("UNUSED", 0, idUNUSED);
	xtMARKER = vm.dict->create("MARKER", 0, idMARKER);
	xtXMARKER = vm.dict->create("(MARKER)", F_HIDDEN, idXMARKER);
	xtPARSE = vm.dict->create("PARSE", 0, idPARSE);
	xtPARSE_WORD = vm.dict->create("PARSE-WORD", 0, idPARSE_WORD);
	xtWORD = vm.dict->create("WORD", 0, idWORD);
	xtCHAR = vm.dict->create("CHAR", 0, idCHAR);
	xtBRACKET_CHAR = vm.dict->create("[CHAR]", F_IMMEDIATE, idBRACKET_CHAR);
	xtNUMBER_Q = vm.dict->create("NUMBER?", 0, idNUMBER_Q);
	xtNUMBER = vm.dict->create("NUMBER", 0, idNUMBER);
	xtTO_NUMBER = vm.dict->create(">NUMBER", 0, idTO_NUMBER);
	xtCONVERT = vm.dict->create("CONVERT", 0, idCONVERT);
	xtCOUNT = vm.dict->create("COUNT", 0, idCOUNT);
	xtDOT_QUOTE = vm.dict->create(".\"", F_IMMEDIATE, idDOT_QUOTE);
	xtXDOT_QUOTE = vm.dict->create("(.\")", F_HIDDEN, idXDOT_QUOTE);
	xtS_QUOTE = vm.dict->create("S\"", F_IMMEDIATE, idS_QUOTE);
	xtXS_QUOTE = vm.dict->create("(S\")", F_HIDDEN, idXS_QUOTE);
	xtC_QUOTE = vm.dict->create("C\"", F_IMMEDIATE, idC_QUOTE);
	xtXC_QUOTE = vm.dict->create("(C\")", F_HIDDEN, idXC_QUOTE);
	xtDOT_PAREN = vm.dict->create(".(", F_IMMEDIATE, idDOT_PAREN);
	xtTIB = vm.dict->create("TIB", 0, idTIB);
	xtNR_IN = vm.dict->create("#IN", 0, idNR_IN);
	xtNR_TIB = vm.dict->create("#TIB", 0, idNR_TIB);
	xtTO_IN = vm.dict->create(">IN", 0, idTO_IN);
	xtSOURCE = vm.dict->create("SOURCE", 0, idSOURCE);
	xtSOURCE_ID = vm.dict->create("SOURCE-ID", 0, idSOURCE_ID);
	xtREFILL = vm.dict->create("REFILL", 0, idREFILL);
	xtACCEPT = vm.dict->create("ACCEPT", 0, idACCEPT);
	xtEXPECT = vm.dict->create("EXPECT", 0, idEXPECT);
	xtKEY = vm.dict->create("KEY", 0, idKEY);
	xtQUERY = vm.dict->create("QUERY", 0, idQUERY);
	xtSAVE_INPUT = vm.dict->create("SAVE-INPUT", 0, idSAVE_INPUT);
	xtRESTORE_INPUT = vm.dict->create("RESTORE-INPUT", 0, idRESTORE_INPUT);
	xtTYPE = vm.dict->create("TYPE", 0, idTYPE);
	xtEMIT = vm.dict->create("EMIT", 0, idEMIT);
	xtCR = vm.dict->create("CR", 0, idCR);
	xtSPACE = vm.dict->create("SPACE", 0, idSPACE);
	xtSPACES = vm.dict->create("SPACES", 0, idSPACES);
	xtLESS_HASH = vm.dict->create("<#", 0, idLESS_HASH);
	xtHASH = vm.dict->create("#", 0, idHASH);
	xtHASH_S = vm.dict->create("#S", 0, idHASH_S);
	xtHOLD = vm.dict->create("HOLD", 0, idHOLD);
	xtSIGN = vm.dict->create("SIGN", 0, idSIGN);
	xtHASH_GREATER = vm.dict->create("#>", 0, idHASH_GREATER);
	xtDOT = vm.dict->create(".", 0, idDOT);
	xtD_DOT = vm.dict->create("D.", 0, idD_DOT);
	xtD_DOT_R = vm.dict->create("D.R", 0, idD_DOT_R);
	xtU_DOT = vm.dict->create("U.", 0, idU_DOT);
	xtDOT_R = vm.dict->create(".R", 0, idDOT_R);
	xtU_DOT_R = vm.dict->create("U.R", 0, idU_DOT_R);
	xtINTERPRET = vm.dict->create("INTERPRET", 0, idINTERPRET);
	xtEVALUATE = vm.dict->create("EVALUATE", 0, idEVALUATE);
	xtEXECUTE = vm.dict->create("EXECUTE", 0, idEXECUTE);
	xtEXIT = vm.dict->create("EXIT", 0, idEXIT);
	xtRDEPTH = vm.dict->create("RDEPTH", 0, idRDEPTH);
	xtCS_DEPTH = vm.dict->create("CS_DEPTH", 0, idCS_DEPTH);
	xtDOT_S = vm.dict->create(".S", 0, idDOT_S);
	xtR_DEPTH = vm.dict->create("RDEPTH", 0, idR_DEPTH);
	xtDOT_RS = vm.dict->create(".RS", 0, idDOT_RS);
	xtBLK = vm.dict->create("BLK", 0, idBLK);
	xtTHROW = vm.dict->create("THROW", 0, idTHROW);
	xtENVIRONMENT_Q = vm.dict->create("ENVIRONMENT?", 0, idENVIRONMENT_Q);
	xtWORDS = vm.dict->create("WORDS", 0, idWORDS);
	xtDABS = vm.dict->create("DABS", 0, idDABS);
	xtBYE = vm.dict->create("BYE", 0, idBYE);
	xtXDOVAR = vm.dict->create("(DOVAR)", F_HIDDEN, idXDOVAR);
	xtLITERAL = vm.dict->create("LITERAL", F_IMMEDIATE, idLITERAL);
	xtXLITERAL = vm.dict->create("(LITERAL)", F_HIDDEN, idXLITERAL);
	xtQUIT = vm.dict->create("QUIT", 0, idQUIT);
	//@@END
}

void f_execute(int xt) {
	bool do_exit = false;
	int old_ip = ip;
	ip = 0;
	while (true) {
		int code = fetch(xt);
		int body = xt + CELL_SZ;			// point to data area, if any

		switch (code) {
		//@@BEGIN: WordsIdExecution
		case idBL: push(BL); break; // BL
		case idS0: push(STACK_SZ); break; // S0
		case idR0: push(STACK_SZ); break; // R0
		case idCS0: push(STACK_SZ); break; // CS0
		case idTRUE: push(F_TRUE); break; // TRUE
		case idFALSE: push(F_FALSE); break; // FALSE
		case idSPAN: push(mem_addr(&vm.user->SPAN)); break; // SPAN
		case idSTATE: push(mem_addr(&vm.user->STATE)); break; // STATE
		case idDPL: push(mem_addr(&vm.user->DPL)); break; // DPL
		case idTRACE: push(mem_addr(&vm.user->TRACE)); break; // TRACE
		case idBASE: push(mem_addr(&vm.user->BASE)); break; // BASE
		case idPAD: { push(mem_addr(vm.pad->pad())); }; break; // PAD
		case idDECIMAL: { vm.user->BASE = 10; }; break; // DECIMAL
		case idHEX: { vm.user->BASE = 16; }; break; // HEX
		case idPLUS: { push(pop() + pop()); }; break; // +
		case idMULT: { push(pop() * pop()); }; break; // *
		case idMINUS: { int b = pop(), a = pop(); push(a - b); }; break; // -
		case idDIV: { int b = pop(), a = pop(); push(f_div(a, b)); }; break; // /
		case idMOD: { int b = pop(), a = pop(); push(f_mod(a, b)); }; break; // MOD
		case idDIV_MOD: { f_div_mod(); }; break; // /MOD
		case idMULT_DIV: { f_mul_div(); }; break; // */
		case idMULT_DIV_MOD: { f_mul_div_mod(); }; break; // */MOD
		case idFM_DIV_MOD: { f_fm_div_mod(); }; break; // FM/MOD
		case idUM_DIV_MOD: { f_um_div_mod(); }; break; // UM/MOD
		case idSM_DIV_REM: { f_sm_div_rem(); }; break; // SM/REM
		case idM_STAR: { dint b = pop(), a = pop(); dpush(a * b); }; break; // M*
		case idONE_PLUS: { push(pop() + 1); }; break; // 1+
		case idONE_MINUS: { push(pop() - 1); }; break; // 1-
		case idTWO_MULT: { push(pop() * 2); }; break; // 2*
		case idTWO_DIV: { push(f_div(pop(), 2)); }; break; // 2/
		case idNEGATE: { push(-pop()); }; break; // NEGATE
		case idS_TO_D: { dpush(pop()); }; break; // S>D
		case idUM_MULT: { udint b = static_cast<uint>(pop()), a = static_cast<uint>(pop()); dpush(a * b); }; break; // UM*
		case idABS: { push(f_abs(pop())); }; break; // ABS
		case idFMAX: { push(f_max(pop(), pop())); }; break; // MAX
		case idFMIN: { push(f_min(pop(), pop())); }; break; // MIN
		case idCHAR_PLUS: { push(pop() + 1); }; break; // CHAR+
		case idCHARS: { push(pop() * 1); }; break; // CHARS
		case idCELL_PLUS: { push(pop() + CELL_SZ); }; break; // CELL+
		case idCELLS: { push(pop() * CELL_SZ); }; break; // CELLS
		case idWITHIN: { f_within(); }; break; // WITHIN
		case idAND: { push(pop() & pop()); }; break; // AND
		case idOR: { push(pop() | pop()); }; break; // OR
		case idXOR: { push(pop() ^ pop()); }; break; // XOR
		case idINVERT: { push(~pop()); }; break; // INVERT
		case idLSHIFT: { int n = pop(), a = pop(); push(a << n); }; break; // LSHIFT
		case idRSHIFT: { int n = pop(), a = pop(); push(a >> n); }; break; // RSHIFT
		case idEQUAL: { push(f_bool(pop() == pop())); }; break; // =
		case idDIFFERENT: { push(f_bool(pop() != pop())); }; break; // <>
		case idLESS: { int b = pop(), a = pop(); push(f_bool(a < b)); }; break; // <
		case idLESS_EQUAL: { int b = pop(), a = pop(); push(f_bool(a <= b)); }; break; // <=
		case idGREATER: { int b = pop(), a = pop(); push(f_bool(a > b)); }; break; // >
		case idGREATER_EQUAL: { int b = pop(), a = pop(); push(f_bool(a >= b)); }; break; // >=
		case idU_LESS: { uint b = pop(), a = pop(); push(f_bool(a < b)); }; break; // U<
		case idU_LESS_EQUAL: { uint b = pop(), a = pop(); push(f_bool(a <= b)); }; break; // U<=
		case idU_GREATER: { uint b = pop(), a = pop(); push(f_bool(a > b)); }; break; // U>
		case idU_GREATER_EQUAL: { uint b = pop(), a = pop(); push(f_bool(a >= b)); }; break; // U>=
		case idZERO_EQUAL: { push(f_bool(pop() == 0)); }; break; // 0=
		case idZERO_DIFFERENT: { push(f_bool(pop() != 0)); }; break; // 0<>
		case idZERO_LESS: { push(f_bool(pop() < 0)); }; break; // 0<
		case idZERO_LESS_EQUAL: { push(f_bool(pop() <= 0)); }; break; // 0<=
		case idZERO_GREATER: { push(f_bool(pop() > 0)); }; break; // 0>
		case idZERO_GREATER_EQUAL: { push(f_bool(pop() >= 0)); }; break; // 0>=
		case idSTORE: { int a = pop(), v = pop(); store(a, v); }; break; // !
		case idFETCH: { push(fetch(pop())); }; break; // @
		case idPLUS_STORE: { int a = pop(), v = pop(); store(a, fetch(a) + v); }; break; // +!
		case idCSTORE: { int a = pop(), v = pop(); cstore(a, v); }; break; // C!
		case idCFETCH: { push(cfetch(pop())); }; break; // C@
		case idTWO_STORE: { int a = pop(); dstore(a, dpop()); }; break; // 2!
		case idTWO_FETCH: { dpush(dfetch(pop())); }; break; // 2@
		case idFILL: { int c = pop(), n = pop(), a = pop(); vm.mem.fill(a, n, c); }; break; // FILL
		case idERASE: { int n = pop(), a = pop(); vm.mem.erase(a, n); }; break; // ERASE
		case idMOVE: { int n = pop(), dst = pop(), src = pop(); vm.mem.move(src, dst, n); }; break; // MOVE
		case idDROP: { pop(); }; break; // DROP
		case idSWAP: { int a = pop(), b = pop(); push(a); push(b); }; break; // SWAP
		case idDUP: { push(peek(0)); }; break; // DUP
		case idQ_DUP: { int a = peek(); if (a) push(a); }; break; // ?DUP
		case idOVER: { push(peek(1)); }; break; // OVER
		case idROT: { int c = pop(), b = pop(), a = pop(); push(b); push(c); push(a); }; break; // ROT
		case idMINUS_ROT: { int c = pop(), b = pop(), a = pop(); push(c); push(a); push(b); }; break; // -ROT
		case idDEPTH: { push(vm.stack->depth()); }; break; // DEPTH
		case idNIP: { int a = pop(); pop(); push(a); }; break; // NIP
		case idPICK: { push(peek(pop())); }; break; // PICK
		case idROLL: { roll(pop()); }; break; // ROLL
		case idTUCK: { int a = pop(), b = pop(); push(a); push(b); push(a); }; break; // TUCK
		case idTWO_DROP: { dpop(); }; break; // 2DROP
		case idTWO_SWAP: { dint a = dpop(), b = dpop(); dpush(a); dpush(b); }; break; // 2SWAP
		case idTWO_DUP: { push(peek(1)); push(peek(1)); }; break; // 2DUP
		case idTWO_OVER: { push(peek(3)); push(peek(3)); }; break; // 2OVER
		case idTWO_ROT: { dint c = dpop(), b = dpop(), a = dpop(); dpush(b); dpush(c); dpush(a); }; break; // 2ROT
		case idMINUS_2ROT: { dint c = dpop(), b = dpop(), a = dpop(); dpush(c); dpush(a); dpush(b); }; break; // -2ROT
		case idSP_FETCH: { push(vm.stack->ptr()); }; break; // SP@
		case idSP_STORE: { vm.stack->set_ptr(pop()); }; break; // SP!
		case idTO_R: { r_push(pop()); }; break; // >R
		case idFROM_R: { push(r_pop()); }; break; // R>
		case idR_DROP: { r_pop(); }; break; // RDROP
		case idRSP_FETCH: { push(vm.rstack->ptr()); }; break; // RSP@
		case idRSP_STORE: { vm.rstack->set_ptr(pop()); }; break; // RSP!
		case idR_FETCH: { push(r_peek(0)); }; break; // R@
		case idI: { push(r_peek(0)); }; break; // I
		case idJ: { push(r_peek(2)); }; break; // J
		case idTWO_TO_R: { r_dpush(dpop()); }; break; // 2>R
		case idTWO_R_TO: { dpush(r_dpop()); }; break; // 2R>
		case idTWO_R_FETCH: { dpush(r_dpeek(0)); }; break; // 2R@
		case idCOMMA: { comma(pop()); }; break; // ,
		case idCCOMMA: { ccomma(pop()); }; break; // C,
		case idHERE: { push(vm.dict->here()); }; break; // HERE
		case idLATEST: { push(vm.dict->latest()); }; break; // LATEST
		case idFIND: { f_find(pop()); }; break; // FIND
		case idTO_BODY: { push(pop() + CELL_SZ); }; break; // >BODY
		case idALIGN: { align(); }; break; // ALIGN
		case idALIGNED: { push(aligned(pop())); }; break; // ALIGNED
		case idALLOT: { vm.dict->allot(pop()); }; break; // ALLOT
		case idUNUSED: { push(vm.dict->unused()); }; break; // UNUSED
		case idMARKER: { f_marker(); }; break; // MARKER
		case idXMARKER: { f_xmarker(body); }; break; // (MARKER)
		case idPARSE: { f_parse(); }; break; // PARSE
		case idPARSE_WORD: { f_parse_word(); }; break; // PARSE-WORD
		case idWORD: { f_word(); }; break; // WORD
		case idCHAR: { f_char(); }; break; // CHAR
		case idBRACKET_CHAR: { f_bracket_char(); }; break; // [CHAR]
		case idNUMBER_Q: { f_number_q(); }; break; // NUMBER?
		case idNUMBER: { f_number(); }; break; // NUMBER
		case idTO_NUMBER: { f_to_number(); }; break; // >NUMBER
		case idCONVERT: { f_convert(); }; break; // CONVERT
		case idCOUNT: { f_count(); }; break; // COUNT
		case idDOT_QUOTE: { f_dot_quote(); }; break; // ."
		case idXDOT_QUOTE: { f_xdot_quote(); }; break; // (.")
		case idS_QUOTE: { f_s_quote(); }; break; // S"
		case idXS_QUOTE: { f_xs_quote(); }; break; // (S")
		case idC_QUOTE: { f_c_quote(); }; break; // C"
		case idXC_QUOTE: { f_xc_quote(); }; break; // (C")
		case idDOT_PAREN: { f_dot_paren(); }; break; // .(
		case idTIB: { f_tib(); }; break; // TIB
		case idNR_IN: { f_nr_in(); }; break; // #IN
		case idNR_TIB: { f_nr_tib(); }; break; // #TIB
		case idTO_IN: { f_to_in(); }; break; // >IN
		case idSOURCE: { f_source(); }; break; // SOURCE
		case idSOURCE_ID: { f_source_id(); }; break; // SOURCE-ID
		case idREFILL: { push(f_bool(f_refill())); }; break; // REFILL
		case idACCEPT: { f_accept(); }; break; // ACCEPT
		case idEXPECT: { f_expect(); }; break; // EXPECT
		case idKEY: { f_key(); }; break; // KEY
		case idQUERY: { f_query(); }; break; // QUERY
		case idSAVE_INPUT: { f_save_input(); }; break; // SAVE-INPUT
		case idRESTORE_INPUT: { f_restore_input(); }; break; // RESTORE-INPUT
		case idTYPE: { int size = pop(), a = pop(); print_string(a, size); }; break; // TYPE
		case idEMIT: { print_char(pop()); }; break; // EMIT
		case idCR: { print_char(CR); }; break; // CR
		case idSPACE: { print_char(BL); }; break; // SPACE
		case idSPACES: { print_spaces(pop()); }; break; // SPACES
		case idLESS_HASH: { vm.number_output->start(); }; break; // <#
		case idHASH: { vm.number_output->add_digit(); }; break; // #
		case idHASH_S: { vm.number_output->add_digits(); }; break; // #S
		case idHOLD: { vm.number_output->add_char(pop()); }; break; // HOLD
		case idSIGN: { vm.number_output->add_sign(pop()); }; break; // SIGN
		case idHASH_GREATER: { vm.number_output->end(); }; break; // #>
		case idDOT: { print_number(pop()); }; break; // .
		case idD_DOT: { print_number(dpop()); }; break; // D.
		case idD_DOT_R: { int w = pop(); dint v = dpop(); print_number(v, w); }; break; // D.R
		case idU_DOT: { print_unsigned_number(pop()); }; break; // U.
		case idDOT_R: { int w = pop(), v = pop(); print_number(v, w); }; break; // .R
		case idU_DOT_R: { int w = pop(); uint v = pop(); print_unsigned_number(v, w); }; break; // U.R
		case idINTERPRET: { f_interpret(); }; break; // INTERPRET
		case idEVALUATE: { f_evaluate(); }; break; // EVALUATE
		case idEXECUTE: { f_execute(pop()); }; break; // EXECUTE
		case idEXIT: { if (r_depth() == 0) do_exit = true; else ip = r_pop(); }; break; // EXIT
		case idRDEPTH: { push(vm.rstack->depth()); }; break; // RDEPTH
		case idCS_DEPTH: { push(vm.cs_stack->depth()); }; break; // CS_DEPTH
		case idDOT_S: { vm.stack->print(); }; break; // .S
		case idR_DEPTH: { push(vm.rstack->depth()); }; break; // RDEPTH
		case idDOT_RS: { vm.rstack->print("R"); }; break; // .RS
		case idBLK: { f_blk(); }; break; // BLK
		case idTHROW: { f_throw(); }; break; // THROW
		case idENVIRONMENT_Q: { int size = pop(), addr = pop(); f_environment_q(mem_char_ptr(addr), size); }; break; // ENVIRONMENT?
		case idWORDS: { f_words(); }; break; // WORDS
		case idDABS: { dpush(f_dabs(dpop())); }; break; // DABS
		case idBYE: { exit(EXIT_SUCCESS); }; break; // BYE
		case idXDOVAR: { ; }; break; // (DOVAR)
		case idLITERAL: { ; }; break; // LITERAL
		case idXLITERAL: { ; }; break; // (LITERAL)
		case idQUIT: { f_quit(); }; break; // QUIT
		//@@END
		default:
			error(Error::InvalidWordXT, std::to_string(xt));
		}

		if (ip == 0 || do_exit)				// ip did not change, exit
			break;

		xt = fetch(ip);
		ip += CELL_SZ;	    // else fetch next xt from ip
	}
	ip = old_ip;
}

// user variables
void User::init() {
	//@@BEGIN: VarsInit
	SPAN = 0;
	STATE = STATE_INTERPRET;
	DPL = 0;
	TRACE = 0;
	BASE = 10;
	//@@END
}

