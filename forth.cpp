//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "interp.h"
#include "math.h"
#include "parser.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

static int body = 0; // TODO: remove this

//@@BEGIN: WordsXtDefinition
int xtBL = 0; // BL
int xtTRUE = 0; // TRUE
int xtFALSE = 0; // FALSE
int xtS0 = 0; // S0
int xtR0 = 0; // R0
int xtCS0 = 0; // CS0
int xtSTATE = 0; // STATE
int xtBASE = 0; // BASE
int xtDPL = 0; // DPL
int xtTRACE = 0; // TRACE
int xtTO_IN = 0; // >IN
int xtNR_IN = 0; // #IN
int xtBLK = 0; // BLK
int xtSOURCE_ID = 0; // SOURCE_ID
int xtPAD = 0; // PAD
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
int xtDEPTH = 0; // DEPTH
int xtSP_FETCH = 0; // SP@
int xtSP_STORE = 0; // SP!
int xtDOT_S = 0; // .S
int xtTO_R = 0; // >R
int xtFROM_R = 0; // R>
int xtR_DROP = 0; // RDROP
int xtR_FETCH = 0; // R@
int xtI = 0; // I
int xtJ = 0; // J
int xtTWO_TO_R = 0; // 2>R
int xtTWO_R_TO = 0; // 2R>
int xtTWO_R_FETCH = 0; // 2R@
int xtR_DEPTH = 0; // RDEPTH
int xtRSP_FETCH = 0; // RSP@
int xtRSP_STORE = 0; // RSP!
int xtDOT_RS = 0; // .RS
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
int xtRDEPTH = 0; // RDEPTH
int xtCS_DEPTH = 0; // CS_DEPTH
int xtTHROW = 0; // THROW
int xtENVIRONMENT_Q = 0; // ENVIRONMENT?
int xtCOUNT = 0; // COUNT
int xtS_QUOTE = 0; // S"
int xtWORD = 0; // WORD
int xtWORDS = 0; // WORDS
int xtDABS = 0; // DABS
int xtDECIMAL = 0; // DECIMAL
int xtHEX = 0; // HEX
int xtBYE = 0; // BYE
int xtXDOVAR = 0; // (DOVAR)
int xtLITERAL = 0; // LITERAL
int xtXLITERAL = 0; // (LITERAL)
int xtINTERPRET = 0; // INTERPRET
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
	xtTRUE = vm.dict->create("TRUE", 0, idTRUE);
	xtFALSE = vm.dict->create("FALSE", 0, idFALSE);
	xtS0 = vm.dict->create("S0", 0, idS0);
	xtR0 = vm.dict->create("R0", 0, idR0);
	xtCS0 = vm.dict->create("CS0", 0, idCS0);
	xtSTATE = vm.dict->create("STATE", 0, idSTATE);
	xtBASE = vm.dict->create("BASE", 0, idBASE);
	xtDPL = vm.dict->create("DPL", 0, idDPL);
	xtTRACE = vm.dict->create("TRACE", 0, idTRACE);
	xtTO_IN = vm.dict->create(">IN", 0, idTO_IN);
	xtNR_IN = vm.dict->create("#IN", 0, idNR_IN);
	xtBLK = vm.dict->create("BLK", 0, idBLK);
	xtSOURCE_ID = vm.dict->create("SOURCE_ID", 0, idSOURCE_ID);
	xtPAD = vm.dict->create("PAD", 0, idPAD);
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
	xtDEPTH = vm.dict->create("DEPTH", 0, idDEPTH);
	xtSP_FETCH = vm.dict->create("SP@", 0, idSP_FETCH);
	xtSP_STORE = vm.dict->create("SP!", 0, idSP_STORE);
	xtDOT_S = vm.dict->create(".S", 0, idDOT_S);
	xtTO_R = vm.dict->create(">R", 0, idTO_R);
	xtFROM_R = vm.dict->create("R>", 0, idFROM_R);
	xtR_DROP = vm.dict->create("RDROP", 0, idR_DROP);
	xtR_FETCH = vm.dict->create("R@", 0, idR_FETCH);
	xtI = vm.dict->create("I", 0, idI);
	xtJ = vm.dict->create("J", 0, idJ);
	xtTWO_TO_R = vm.dict->create("2>R", 0, idTWO_TO_R);
	xtTWO_R_TO = vm.dict->create("2R>", 0, idTWO_R_TO);
	xtTWO_R_FETCH = vm.dict->create("2R@", 0, idTWO_R_FETCH);
	xtR_DEPTH = vm.dict->create("RDEPTH", 0, idR_DEPTH);
	xtRSP_FETCH = vm.dict->create("RSP@", 0, idRSP_FETCH);
	xtRSP_STORE = vm.dict->create("RSP!", 0, idRSP_STORE);
	xtDOT_RS = vm.dict->create(".RS", 0, idDOT_RS);
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
	xtRDEPTH = vm.dict->create("RDEPTH", 0, idRDEPTH);
	xtCS_DEPTH = vm.dict->create("CS_DEPTH", 0, idCS_DEPTH);
	xtTHROW = vm.dict->create("THROW", 0, idTHROW);
	xtENVIRONMENT_Q = vm.dict->create("ENVIRONMENT?", 0, idENVIRONMENT_Q);
	xtCOUNT = vm.dict->create("COUNT", 0, idCOUNT);
	xtS_QUOTE = vm.dict->create("S\"", 0, idS_QUOTE);
	xtWORD = vm.dict->create("WORD", 0, idWORD);
	xtWORDS = vm.dict->create("WORDS", 0, idWORDS);
	xtDABS = vm.dict->create("DABS", 0, idDABS);
	xtDECIMAL = vm.dict->create("DECIMAL", 0, idDECIMAL);
	xtHEX = vm.dict->create("HEX", 0, idHEX);
	xtBYE = vm.dict->create("BYE", 0, idBYE);
	xtXDOVAR = vm.dict->create("(DOVAR)", F_HIDDEN, idXDOVAR);
	xtLITERAL = vm.dict->create("LITERAL", F_IMMEDIATE, idLITERAL);
	xtXLITERAL = vm.dict->create("(LITERAL)", F_HIDDEN, idXLITERAL);
	xtINTERPRET = vm.dict->create("INTERPRET", 0, idINTERPRET);
	xtQUIT = vm.dict->create("QUIT", 0, idQUIT);
	//@@END
}

void f_execute(int xt) {
	int code = fetch(xt);
	if (code < 0) {
		error(Error::InvalidWordId, std::to_string(xt));
	}
	else if (code < MAX_WORD_ID) {
		switch (code) {
		//@@BEGIN: WordsIdExecution
		case idBL: fBL(); break; // BL
		case idTRUE: fTRUE(); break; // TRUE
		case idFALSE: fFALSE(); break; // FALSE
		case idS0: fS0(); break; // S0
		case idR0: fR0(); break; // R0
		case idCS0: fCS0(); break; // CS0
		case idSTATE: fSTATE(); break; // STATE
		case idBASE: fBASE(); break; // BASE
		case idDPL: fDPL(); break; // DPL
		case idTRACE: fTRACE(); break; // TRACE
		case idTO_IN: fTO_IN(); break; // >IN
		case idNR_IN: fNR_IN(); break; // #IN
		case idBLK: fBLK(); break; // BLK
		case idSOURCE_ID: fSOURCE_ID(); break; // SOURCE_ID
		case idPAD: fPAD(); break; // PAD
		case idPLUS: fPLUS(); break; // +
		case idMULT: fMULT(); break; // *
		case idMINUS: fMINUS(); break; // -
		case idDIV: fDIV(); break; // /
		case idMOD: fMOD(); break; // MOD
		case idDIV_MOD: fDIV_MOD(); break; // /MOD
		case idMULT_DIV: fMULT_DIV(); break; // */
		case idMULT_DIV_MOD: fMULT_DIV_MOD(); break; // */MOD
		case idFM_DIV_MOD: fFM_DIV_MOD(); break; // FM/MOD
		case idUM_DIV_MOD: fUM_DIV_MOD(); break; // UM/MOD
		case idSM_DIV_REM: fSM_DIV_REM(); break; // SM/REM
		case idM_STAR: fM_STAR(); break; // M*
		case idONE_PLUS: fONE_PLUS(); break; // 1+
		case idONE_MINUS: fONE_MINUS(); break; // 1-
		case idTWO_MULT: fTWO_MULT(); break; // 2*
		case idTWO_DIV: fTWO_DIV(); break; // 2/
		case idNEGATE: fNEGATE(); break; // NEGATE
		case idS_TO_D: fS_TO_D(); break; // S>D
		case idUM_MULT: fUM_MULT(); break; // UM*
		case idABS: fABS(); break; // ABS
		case idFMAX: fFMAX(); break; // MAX
		case idFMIN: fFMIN(); break; // MIN
		case idCHAR_PLUS: fCHAR_PLUS(); break; // CHAR+
		case idCHARS: fCHARS(); break; // CHARS
		case idCELL_PLUS: fCELL_PLUS(); break; // CELL+
		case idCELLS: fCELLS(); break; // CELLS
		case idWITHIN: fWITHIN(); break; // WITHIN
		case idAND: fAND(); break; // AND
		case idOR: fOR(); break; // OR
		case idXOR: fXOR(); break; // XOR
		case idINVERT: fINVERT(); break; // INVERT
		case idLSHIFT: fLSHIFT(); break; // LSHIFT
		case idRSHIFT: fRSHIFT(); break; // RSHIFT
		case idEQUAL: fEQUAL(); break; // =
		case idDIFFERENT: fDIFFERENT(); break; // <>
		case idLESS: fLESS(); break; // <
		case idLESS_EQUAL: fLESS_EQUAL(); break; // <=
		case idGREATER: fGREATER(); break; // >
		case idGREATER_EQUAL: fGREATER_EQUAL(); break; // >=
		case idU_LESS: fU_LESS(); break; // U<
		case idU_LESS_EQUAL: fU_LESS_EQUAL(); break; // U<=
		case idU_GREATER: fU_GREATER(); break; // U>
		case idU_GREATER_EQUAL: fU_GREATER_EQUAL(); break; // U>=
		case idZERO_EQUAL: fZERO_EQUAL(); break; // 0=
		case idZERO_DIFFERENT: fZERO_DIFFERENT(); break; // 0<>
		case idZERO_LESS: fZERO_LESS(); break; // 0<
		case idZERO_LESS_EQUAL: fZERO_LESS_EQUAL(); break; // 0<=
		case idZERO_GREATER: fZERO_GREATER(); break; // 0>
		case idZERO_GREATER_EQUAL: fZERO_GREATER_EQUAL(); break; // 0>=
		case idSTORE: fSTORE(); break; // !
		case idFETCH: fFETCH(); break; // @
		case idPLUS_STORE: fPLUS_STORE(); break; // +!
		case idCSTORE: fCSTORE(); break; // C!
		case idCFETCH: fCFETCH(); break; // C@
		case idTWO_STORE: fTWO_STORE(); break; // 2!
		case idTWO_FETCH: fTWO_FETCH(); break; // 2@
		case idFILL: fFILL(); break; // FILL
		case idERASE: fERASE(); break; // ERASE
		case idMOVE: fMOVE(); break; // MOVE
		case idDROP: fDROP(); break; // DROP
		case idSWAP: fSWAP(); break; // SWAP
		case idDUP: fDUP(); break; // DUP
		case idQ_DUP: fQ_DUP(); break; // ?DUP
		case idOVER: fOVER(); break; // OVER
		case idROT: fROT(); break; // ROT
		case idMINUS_ROT: fMINUS_ROT(); break; // -ROT
		case idNIP: fNIP(); break; // NIP
		case idPICK: fPICK(); break; // PICK
		case idROLL: fROLL(); break; // ROLL
		case idTUCK: fTUCK(); break; // TUCK
		case idTWO_DROP: fTWO_DROP(); break; // 2DROP
		case idTWO_SWAP: fTWO_SWAP(); break; // 2SWAP
		case idTWO_DUP: fTWO_DUP(); break; // 2DUP
		case idTWO_OVER: fTWO_OVER(); break; // 2OVER
		case idTWO_ROT: fTWO_ROT(); break; // 2ROT
		case idMINUS_2ROT: fMINUS_2ROT(); break; // -2ROT
		case idDEPTH: fDEPTH(); break; // DEPTH
		case idSP_FETCH: fSP_FETCH(); break; // SP@
		case idSP_STORE: fSP_STORE(); break; // SP!
		case idDOT_S: fDOT_S(); break; // .S
		case idTO_R: fTO_R(); break; // >R
		case idFROM_R: fFROM_R(); break; // R>
		case idR_DROP: fR_DROP(); break; // RDROP
		case idR_FETCH: fR_FETCH(); break; // R@
		case idI: fI(); break; // I
		case idJ: fJ(); break; // J
		case idTWO_TO_R: fTWO_TO_R(); break; // 2>R
		case idTWO_R_TO: fTWO_R_TO(); break; // 2R>
		case idTWO_R_FETCH: fTWO_R_FETCH(); break; // 2R@
		case idR_DEPTH: fR_DEPTH(); break; // RDEPTH
		case idRSP_FETCH: fRSP_FETCH(); break; // RSP@
		case idRSP_STORE: fRSP_STORE(); break; // RSP!
		case idDOT_RS: fDOT_RS(); break; // .RS
		case idCOMMA: fCOMMA(); break; // ,
		case idCCOMMA: fCCOMMA(); break; // C,
		case idHERE: fHERE(); break; // HERE
		case idLATEST: fLATEST(); break; // LATEST
		case idFIND: fFIND(); break; // FIND
		case idTO_BODY: fTO_BODY(); break; // >BODY
		case idALIGN: fALIGN(); break; // ALIGN
		case idALIGNED: fALIGNED(); break; // ALIGNED
		case idALLOT: fALLOT(); break; // ALLOT
		case idUNUSED: fUNUSED(); break; // UNUSED
		case idMARKER: fMARKER(); break; // MARKER
		case idXMARKER: fXMARKER(); break; // (MARKER)
		case idTYPE: fTYPE(); break; // TYPE
		case idEMIT: fEMIT(); break; // EMIT
		case idCR: fCR(); break; // CR
		case idSPACE: fSPACE(); break; // SPACE
		case idSPACES: fSPACES(); break; // SPACES
		case idLESS_HASH: fLESS_HASH(); break; // <#
		case idHASH: fHASH(); break; // #
		case idHASH_S: fHASH_S(); break; // #S
		case idHOLD: fHOLD(); break; // HOLD
		case idSIGN: fSIGN(); break; // SIGN
		case idHASH_GREATER: fHASH_GREATER(); break; // #>
		case idDOT: fDOT(); break; // .
		case idD_DOT: fD_DOT(); break; // D.
		case idD_DOT_R: fD_DOT_R(); break; // D.R
		case idU_DOT: fU_DOT(); break; // U.
		case idDOT_R: fDOT_R(); break; // .R
		case idU_DOT_R: fU_DOT_R(); break; // U.R
		case idRDEPTH: fRDEPTH(); break; // RDEPTH
		case idCS_DEPTH: fCS_DEPTH(); break; // CS_DEPTH
		case idTHROW: fTHROW(); break; // THROW
		case idENVIRONMENT_Q: fENVIRONMENT_Q(); break; // ENVIRONMENT?
		case idCOUNT: fCOUNT(); break; // COUNT
		case idS_QUOTE: fS_QUOTE(); break; // S"
		case idWORD: fWORD(); break; // WORD
		case idWORDS: fWORDS(); break; // WORDS
		case idDABS: fDABS(); break; // DABS
		case idDECIMAL: fDECIMAL(); break; // DECIMAL
		case idHEX: fHEX(); break; // HEX
		case idBYE: fBYE(); break; // BYE
		case idXDOVAR: fXDOVAR(); break; // (DOVAR)
		case idLITERAL: fLITERAL(); break; // LITERAL
		case idXLITERAL: fXLITERAL(); break; // (LITERAL)
		case idINTERPRET: fINTERPRET(); break; // INTERPRET
		case idQUIT: fQUIT(); break; // QUIT
		//@@END
		default:
			assert(0); // not reached
		}
	}
	else if (xt > vm.dict->here()) {
		error(Error::InvalidWordId, std::to_string(xt));
	}
	else {
		// TODO: execute Forth word at code
		//Header* header = Header::header(xt);
		//ForthString* name = header->name();
	}
}

// user variables
void User::init() {
	//@@BEGIN: VarsInit
	STATE = STATE_INTERPRET;
	BASE = 10;
	DPL = 0;
	TRACE = 0;
	TO_IN = 0;
	NR_IN = 0;
	BLK = 0;
	SOURCE_ID = -1;
	//@@END
}

//@@BEGIN: ConstImplementation
// BL
void fBL() {
	push(BL);
}

// TRUE
void fTRUE() {
	push(F_TRUE);
}

// FALSE
void fFALSE() {
	push(F_FALSE);
}

// S0
void fS0() {
	push(STACK_SZ);
}

// R0
void fR0() {
	push(STACK_SZ);
}

// CS0
void fCS0() {
	push(STACK_SZ);
}

//@@END

//@@BEGIN: VarsImplementation
// STATE
void fSTATE() {
	push(mem_addr(&vm.user->STATE));
}

// BASE
void fBASE() {
	push(mem_addr(&vm.user->BASE));
}

// DPL
void fDPL() {
	push(mem_addr(&vm.user->DPL));
}

// TRACE
void fTRACE() {
	push(mem_addr(&vm.user->TRACE));
}

// >IN
void fTO_IN() {
	push(mem_addr(&vm.user->TO_IN));
}

// #IN
void fNR_IN() {
	push(mem_addr(&vm.user->NR_IN));
}

// BLK
void fBLK() {
	push(mem_addr(&vm.user->BLK));
}

// SOURCE_ID
void fSOURCE_ID() {
	push(mem_addr(&vm.user->SOURCE_ID));
}

//@@END

//@@BEGIN: WordsImplementation
// PAD
void fPAD() {
	push(mem_addr(vm.pad->pad()));
}

// +
void fPLUS() {
	push(pop() + pop());
}

// *
void fMULT() {
	push(pop() * pop());
}

// -
void fMINUS() {
	int b = pop(), a = pop(); push(a - b);
}

// /
void fDIV() {
	int b = pop(), a = pop(); push(f_div(a, b));
}

// MOD
void fMOD() {
	int b = pop(), a = pop(); push(f_mod(a, b));
}

// /MOD
void fDIV_MOD() {
	f_div_mod();
}

// */
void fMULT_DIV() {
	f_mul_div();
}

// */MOD
void fMULT_DIV_MOD() {
	f_mul_div_mod();
}

// FM/MOD
void fFM_DIV_MOD() {
	f_fm_div_mod();
}

// UM/MOD
void fUM_DIV_MOD() {
	f_um_div_mod();
}

// SM/REM
void fSM_DIV_REM() {
	f_sm_div_rem();
}

// M*
void fM_STAR() {
	dint b = pop(), a = pop(); dpush(a * b);
}

// 1+
void fONE_PLUS() {
	push(pop() + 1);
}

// 1-
void fONE_MINUS() {
	push(pop() - 1);
}

// 2*
void fTWO_MULT() {
	push(pop() * 2);
}

// 2/
void fTWO_DIV() {
	push(f_div(pop(), 2));
}

// NEGATE
void fNEGATE() {
	push(-pop());
}

// S>D
void fS_TO_D() {
	dpush(pop());
}

// UM*
void fUM_MULT() {
	udint b = static_cast<uint>(pop()), a = static_cast<uint>(pop()); dpush(a * b);
}

// ABS
void fABS() {
	push(f_abs(pop()));
}

// MAX
void fFMAX() {
	push(f_max(pop(), pop()));
}

// MIN
void fFMIN() {
	push(f_min(pop(), pop()));
}

// CHAR+
void fCHAR_PLUS() {
	push(pop() + 1);
}

// CHARS
void fCHARS() {
	push(pop() * 1);
}

// CELL+
void fCELL_PLUS() {
	push(pop() + CELL_SZ);
}

// CELLS
void fCELLS() {
	push(pop() * CELL_SZ);
}

// WITHIN
void fWITHIN() {
	f_within();
}

// AND
void fAND() {
	push(pop() & pop());
}

// OR
void fOR() {
	push(pop() | pop());
}

// XOR
void fXOR() {
	push(pop() ^ pop());
}

// INVERT
void fINVERT() {
	push(~pop());
}

// LSHIFT
void fLSHIFT() {
	int n = pop(), a = pop(); push(a << n);
}

// RSHIFT
void fRSHIFT() {
	int n = pop(), a = pop(); push(a >> n);
}

// =
void fEQUAL() {
	push(f_bool(pop() == pop()));
}

// <>
void fDIFFERENT() {
	push(f_bool(pop() != pop()));
}

// <
void fLESS() {
	int b = pop(), a = pop(); push(f_bool(a < b));
}

// <=
void fLESS_EQUAL() {
	int b = pop(), a = pop(); push(f_bool(a <= b));
}

// >
void fGREATER() {
	int b = pop(), a = pop(); push(f_bool(a > b));
}

// >=
void fGREATER_EQUAL() {
	int b = pop(), a = pop(); push(f_bool(a >= b));
}

// U<
void fU_LESS() {
	uint b = pop(), a = pop(); push(f_bool(a < b));
}

// U<=
void fU_LESS_EQUAL() {
	uint b = pop(), a = pop(); push(f_bool(a <= b));
}

// U>
void fU_GREATER() {
	uint b = pop(), a = pop(); push(f_bool(a > b));
}

// U>=
void fU_GREATER_EQUAL() {
	uint b = pop(), a = pop(); push(f_bool(a >= b));
}

// 0=
void fZERO_EQUAL() {
	push(f_bool(pop() == 0));
}

// 0<>
void fZERO_DIFFERENT() {
	push(f_bool(pop() != 0));
}

// 0<
void fZERO_LESS() {
	push(f_bool(pop() < 0));
}

// 0<=
void fZERO_LESS_EQUAL() {
	push(f_bool(pop() <= 0));
}

// 0>
void fZERO_GREATER() {
	push(f_bool(pop() > 0));
}

// 0>=
void fZERO_GREATER_EQUAL() {
	push(f_bool(pop() >= 0));
}

// !
void fSTORE() {
	int a = pop(), v = pop(); store(a, v);
}

// @
void fFETCH() {
	push(fetch(pop()));
}

// +!
void fPLUS_STORE() {
	int a = pop(), v = pop(); store(a, fetch(a) + v);
}

// C!
void fCSTORE() {
	int a = pop(), v = pop(); cstore(a, v);
}

// C@
void fCFETCH() {
	push(cfetch(pop()));
}

// 2!
void fTWO_STORE() {
	int a = pop(); dstore(a, dpop());
}

// 2@
void fTWO_FETCH() {
	dpush(dfetch(pop()));
}

// FILL
void fFILL() {
	int c = pop(), n = pop(), a = pop(); vm.mem.fill(a, n, c);
}

// ERASE
void fERASE() {
	int n = pop(), a = pop(); vm.mem.erase(a, n);
}

// MOVE
void fMOVE() {
	int n = pop(), dst = pop(), src = pop(); vm.mem.move(src, dst, n);
}

// DROP
void fDROP() {
	pop();
}

// SWAP
void fSWAP() {
	int a = pop(), b = pop(); push(a); push(b);
}

// DUP
void fDUP() {
	push(peek(0));
}

// ?DUP
void fQ_DUP() {
	int a = peek(); if (a) push(a);
}

// OVER
void fOVER() {
	push(peek(1));
}

// ROT
void fROT() {
	int c = pop(), b = pop(), a = pop(); push(b); push(c); push(a);
}

// -ROT
void fMINUS_ROT() {
	int c = pop(), b = pop(), a = pop(); push(c); push(a); push(b);
}

// NIP
void fNIP() {
	int a = pop(); pop(); push(a);
}

// PICK
void fPICK() {
	push(peek(pop()));
}

// ROLL
void fROLL() {
	roll(pop());
}

// TUCK
void fTUCK() {
	int a = pop(), b = pop(); push(a); push(b); push(a);
}

// 2DROP
void fTWO_DROP() {
	dpop();
}

// 2SWAP
void fTWO_SWAP() {
	dint a = dpop(), b = dpop(); dpush(a); dpush(b);
}

// 2DUP
void fTWO_DUP() {
	push(peek(1)); push(peek(1));
}

// 2OVER
void fTWO_OVER() {
	push(peek(3)); push(peek(3));
}

// 2ROT
void fTWO_ROT() {
	dint c = dpop(), b = dpop(), a = dpop(); dpush(b); dpush(c); dpush(a);
}

// -2ROT
void fMINUS_2ROT() {
	dint c = dpop(), b = dpop(), a = dpop(); dpush(c); dpush(a); dpush(b);
}

// DEPTH
void fDEPTH() {
	push(vm.stack->depth());
}

// SP@
void fSP_FETCH() {
	push(vm.stack->ptr());
}

// SP!
void fSP_STORE() {
	vm.stack->set_ptr(pop());
}

// .S
void fDOT_S() {
	vm.stack->print();
}

// >R
void fTO_R() {
	r_push(pop());
}

// R>
void fFROM_R() {
	push(r_pop());
}

// RDROP
void fR_DROP() {
	r_pop();
}

// R@
void fR_FETCH() {
	push(r_peek(0));
}

// I
void fI() {
	push(r_peek(0));
}

// J
void fJ() {
	push(r_peek(2));
}

// 2>R
void fTWO_TO_R() {
	r_dpush(dpop());
}

// 2R>
void fTWO_R_TO() {
	dpush(r_dpop());
}

// 2R@
void fTWO_R_FETCH() {
	dpush(r_dpeek(0));
}

// RDEPTH
void fR_DEPTH() {
	push(vm.rstack->depth());
}

// RSP@
void fRSP_FETCH() {
	push(vm.rstack->ptr());
}

// RSP!
void fRSP_STORE() {
	vm.rstack->set_ptr(pop());
}

// .RS
void fDOT_RS() {
	vm.rstack->print("R");
}

// ,
void fCOMMA() {
	comma(pop());
}

// C,
void fCCOMMA() {
	ccomma(pop());
}

// HERE
void fHERE() {
	push(vm.dict->here());
}

// LATEST
void fLATEST() {
	push(vm.dict->latest());
}

// FIND
void fFIND() {
	f_find(pop());
}

// >BODY
void fTO_BODY() {
	push(pop() + CELL_SZ);
}

// ALIGN
void fALIGN() {
	align();
}

// ALIGNED
void fALIGNED() {
	push(aligned(pop()));
}

// ALLOT
void fALLOT() {
	vm.dict->allot(pop());
}

// UNUSED
void fUNUSED() {
	push(vm.dict->unused());
}

// MARKER
void fMARKER() {
	f_marker();
}

// (MARKER)
void fXMARKER() {
	f_xmarker(body);
}

// TYPE
void fTYPE() {
	int size = pop(), a = pop(); print_string(a, size);
}

// EMIT
void fEMIT() {
	print_char(pop());
}

// CR
void fCR() {
	print_char(CR);
}

// SPACE
void fSPACE() {
	print_char(BL);
}

// SPACES
void fSPACES() {
	print_spaces(pop());
}

// <#
void fLESS_HASH() {
	vm.number_output->start();
}

// #
void fHASH() {
	vm.number_output->add_digit();
}

// #S
void fHASH_S() {
	vm.number_output->add_digits();
}

// HOLD
void fHOLD() {
	vm.number_output->add_char(pop());
}

// SIGN
void fSIGN() {
	vm.number_output->add_sign(pop());
}

// #>
void fHASH_GREATER() {
	vm.number_output->end();
}

// .
void fDOT() {
	print_number(pop());
}

// D.
void fD_DOT() {
	print_number(dpop());
}

// D.R
void fD_DOT_R() {
	int w = pop(); dint v = dpop(); print_number(v, w);
}

// U.
void fU_DOT() {
	print_unsigned_number(pop());
}

// .R
void fDOT_R() {
	int w = pop(), v = pop(); print_number(v, w);
}

// U.R
void fU_DOT_R() {
	int w = pop(); uint v = pop(); print_unsigned_number(v, w);
}

// RDEPTH
void fRDEPTH() {
	push(vm.rstack->depth());
}

// CS_DEPTH
void fCS_DEPTH() {
	push(vm.cs_stack->depth());
}

// DECIMAL
void fDECIMAL() {
	vm.user->BASE = 10;
}

// HEX
void fHEX() {
	vm.user->BASE = 16;
}

// BYE
void fBYE() {
	exit(EXIT_SUCCESS);
}

// (DOVAR)
void fXDOVAR() {
	;
}

// LITERAL
void fLITERAL() {
	;
}

// (LITERAL)
void fXLITERAL() {
	;
}

// INTERPRET
void fINTERPRET() {
	f_interpret();
}

// QUIT
void fQUIT() {
	f_quit();
}

//@@END

void fENVIRONMENT_Q() {
	int size = pop();
	int addr = pop();
	const char* str = mem_char_ptr(addr);
	string query{ str, str + size };
	if (query == "CORE") {
		push(F_TRUE);
		push(F_TRUE);
	}
	else {
		push(F_FALSE);
	}
}

void fDABS() {
	dint a = f_dabs(dpop());
	dpush(a);
}

void fWORDS() {
	vector<string> words = vm.dict->get_words();
	size_t col = 0;
	for (auto& word : words) {
		if (col + 1 + word.size() >= SCREEN_WIDTH) {
			cout << endl << word;
			col = word.size();
		}
		else if (col == 0) {
			cout << word;
			col += word.size();
		}
		else {
			cout << BL << word;
			col += 1 + word.size();
		}
	}
	cout << endl;
}

void fTHROW() {
	int error_code = pop();
	error(static_cast<Error>(error_code));
}

void fCOUNT() {
	int addr = pop();
	int len = cfetch(addr++);
	push(addr);
	push(len);
}

void fWORD() {
	char delimiter = pop();
	const ForthString* word = f_parse_word(delimiter);
	if (word == nullptr)
		exit(EXIT_SUCCESS);		        // no more input
	else {
		const CountedString* addr = word->counted_string();
		push(mem_addr(reinterpret_cast<const char*>(addr)));	// address counted string
	}
}

void fS_QUOTE() {
	const ForthString* word = f_parse_word('"');
	if (word == nullptr)
		exit(EXIT_SUCCESS);		        // no more input
	else {
		push(mem_addr(word->str()));	// address of word
		push(word->size());				// length of word
	}
}
