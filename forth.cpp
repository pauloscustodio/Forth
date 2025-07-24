//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "parser.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

//@@BEGIN: WordsXtDefinition
int xtTRUE = 0; // TRUE
int xtFALSE = 0; // FALSE
int xtS0 = 0; // S0
int xtR0 = 0; // R0
int xtSTRUCT0 = 0; // STRUCT0
int xtBASE = 0; // BASE
int xtSTATE = 0; // STATE
int xtDPL = 0; // DPL
int xtTRACE = 0; // TRACE
int xtSTORE = 0; // !
int xtFETCH = 0; // @
int xtC_STORE = 0; // C!
int xtC_FETCH = 0; // C@
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
int xtDEPTH = 0; // DEPTH
int xtRDEPTH = 0; // RDEPTH
int xtSDEPTH = 0; // SDEPTH
int xtPAD = 0; // PAD
int xtTHROW = 0; // THROW
int xtDROP = 0; // DROP
int xtDUP = 0; // DUP
int xtPICK = 0; // PICK
int xtPLUS = 0; // +
int xtENVIRONMENT_Q = 0; // ENVIRONMENT?
int xtCOUNT = 0; // COUNT
int xtS_QUOTE = 0; // S"
int xtDOT_S = 0; // .S
int xtWORD = 0; // WORD
int xtWORDS = 0; // WORDS
int xtFIND = 0; // FIND
int xtCOMMA = 0; // ,
int xtC_COMMA = 0; // C,
int xtALIGN = 0; // ALIGN
//@@END

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

int dcell(int hi, int lo) {
	return (static_cast<udint>(dcell_lo(hi)) << 32) |
		static_cast<udint>(dcell_lo(lo));
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

void dpush(dint value) {
	vm.stack->dpush(value);
}

dint dpop() {
	return vm.stack->dpop();
}

dint dpeek(int depth) {
	return vm.stack->dpeek(depth);
}

void spush(int value) {
	vm.sstack->push(value);
}

int spop() {
	return vm.sstack->pop();
}

int speek(int depth) {
	return vm.sstack->peek(depth);
}

void rpush(int value) {
	vm.rstack->push(value);
}

int rpop() {
	return vm.rstack->pop();
}

int rpeek(int depth) {
	return vm.rstack->peek(depth);
}

void create_dictionary() {
	//@@BEGIN: WordsCreateDictionary
	xtTRUE = vm.dict->create("TRUE", 0, idTRUE);
	xtFALSE = vm.dict->create("FALSE", 0, idFALSE);
	xtS0 = vm.dict->create("S0", 0, idS0);
	xtR0 = vm.dict->create("R0", 0, idR0);
	xtSTRUCT0 = vm.dict->create("STRUCT0", 0, idSTRUCT0);
	xtBASE = vm.dict->create("BASE", 0, idBASE);
	xtSTATE = vm.dict->create("STATE", 0, idSTATE);
	xtDPL = vm.dict->create("DPL", 0, idDPL);
	xtTRACE = vm.dict->create("TRACE", 0, idTRACE);
	xtSTORE = vm.dict->create("!", 0, idSTORE);
	xtFETCH = vm.dict->create("@", 0, idFETCH);
	xtC_STORE = vm.dict->create("C!", 0, idC_STORE);
	xtC_FETCH = vm.dict->create("C@", 0, idC_FETCH);
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
	xtDEPTH = vm.dict->create("DEPTH", 0, idDEPTH);
	xtRDEPTH = vm.dict->create("RDEPTH", 0, idRDEPTH);
	xtSDEPTH = vm.dict->create("SDEPTH", 0, idSDEPTH);
	xtPAD = vm.dict->create("PAD", 0, idPAD);
	xtTHROW = vm.dict->create("THROW", 0, idTHROW);
	xtDROP = vm.dict->create("DROP", 0, idDROP);
	xtDUP = vm.dict->create("DUP", 0, idDUP);
	xtPICK = vm.dict->create("PICK", 0, idPICK);
	xtPLUS = vm.dict->create("+", 0, idPLUS);
	xtENVIRONMENT_Q = vm.dict->create("ENVIRONMENT?", 0, idENVIRONMENT_Q);
	xtCOUNT = vm.dict->create("COUNT", 0, idCOUNT);
	xtS_QUOTE = vm.dict->create("S\"", 0, idS_QUOTE);
	xtDOT_S = vm.dict->create(".S", 0, idDOT_S);
	xtWORD = vm.dict->create("WORD", 0, idWORD);
	xtWORDS = vm.dict->create("WORDS", 0, idWORDS);
	xtFIND = vm.dict->create("FIND", 0, idFIND);
	xtCOMMA = vm.dict->create(",", 0, idCOMMA);
	xtC_COMMA = vm.dict->create("C,", 0, idC_COMMA);
	xtALIGN = vm.dict->create("ALIGN", 0, idALIGN);
	//@@END
}

void execute_word(int xt) {
	int code = fetch(xt);
	if (code < 0) {
		error(Error::InvalidWordId, std::to_string(xt));
	}
	else if (code < MAX_WORD_ID) {
		switch (code) {
		//@@BEGIN: WordsIdExecution
		case idTRUE: fTRUE(); break; // TRUE
		case idFALSE: fFALSE(); break; // FALSE
		case idS0: fS0(); break; // S0
		case idR0: fR0(); break; // R0
		case idSTRUCT0: fSTRUCT0(); break; // STRUCT0
		case idBASE: fBASE(); break; // BASE
		case idSTATE: fSTATE(); break; // STATE
		case idDPL: fDPL(); break; // DPL
		case idTRACE: fTRACE(); break; // TRACE
		case idSTORE: fSTORE(); break; // !
		case idFETCH: fFETCH(); break; // @
		case idC_STORE: fC_STORE(); break; // C!
		case idC_FETCH: fC_FETCH(); break; // C@
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
		case idDEPTH: fDEPTH(); break; // DEPTH
		case idRDEPTH: fRDEPTH(); break; // RDEPTH
		case idSDEPTH: fSDEPTH(); break; // SDEPTH
		case idPAD: fPAD(); break; // PAD
		case idTHROW: fTHROW(); break; // THROW
		case idDROP: fDROP(); break; // DROP
		case idDUP: fDUP(); break; // DUP
		case idPICK: fPICK(); break; // PICK
		case idPLUS: fPLUS(); break; // +
		case idENVIRONMENT_Q: fENVIRONMENT_Q(); break; // ENVIRONMENT?
		case idCOUNT: fCOUNT(); break; // COUNT
		case idS_QUOTE: fS_QUOTE(); break; // S"
		case idDOT_S: fDOT_S(); break; // .S
		case idWORD: fWORD(); break; // WORD
		case idWORDS: fWORDS(); break; // WORDS
		case idFIND: fFIND(); break; // FIND
		case idCOMMA: fCOMMA(); break; // ,
		case idC_COMMA: fC_COMMA(); break; // C,
		case idALIGN: fALIGN(); break; // ALIGN
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
	BASE = 10;
	STATE = STATE_INTERPRET;
	DPL = 0;
	TRACE = 0;
	//@@END
}

//@@BEGIN: ConstImplementation
void fTRUE() { push(F_TRUE); } // TRUE
void fFALSE() { push(F_FALSE); } // FALSE
void fS0() { push(STACK_SZ); } // S0
void fR0() { push(STACK_SZ); } // R0
void fSTRUCT0() { push(STACK_SZ); } // STRUCT0
//@@END

//@@BEGIN: VarsImplementation
void fBASE() { push(mem_addr(&vm.user->BASE)); } // BASE
void fSTATE() { push(mem_addr(&vm.user->STATE)); } // STATE
void fDPL() { push(mem_addr(&vm.user->DPL)); } // DPL
void fTRACE() { push(mem_addr(&vm.user->TRACE)); } // TRACE
//@@END

//-----------------------------------------------------------------------------
// CORE set
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Unsorted words
//-----------------------------------------------------------------------------

// memory access
void fSTORE() {
	int addr = pop();
	int value = pop();
	store(addr, value);
}

void fFETCH() {
	int addr = pop();
	int value = fetch(addr);
	push(value);
}

void fC_STORE() {
	int addr = pop();
	int value = pop();
	cstore(addr, value);
}

void fC_FETCH() {
	int addr = pop();
	int value = cfetch(addr);
	push(value);
}

// string output
void fTYPE() {
	int size = pop();
	int addr = pop();
	print_string(addr, size);
}

void fEMIT() {
	char c = static_cast<char>(pop());
	cout << c;
}

void fCR() {
	cout << endl;
}

void fSPACE() {
	cout << BL;
}

void fSPACES() {
	int size = pop();
	for (int i = 0; i < size; ++i)
		cout << BL;
}

// formatted number output
void fLESS_HASH() {
	vm.number_output->start();
}

void fHASH() {
	vm.number_output->add_digit();
}

void fHASH_S() {
	vm.number_output->add_digits();
}

void fHOLD() {
	char c = static_cast<char>(pop());
	vm.number_output->add_char(c);
}

void fSIGN() {
	int sign = pop();
	vm.number_output->add_sign(sign);
}

void fHASH_GREATER() {
	vm.number_output->end();
}

// number output
void fDOT() {
	int value = pop();
	print_number(value);
}

void fD_DOT() {
	dint value = dpop();
	print_number(value);
}

void fD_DOT_R() {
	int width = pop();
	dint value = dpop();
	print_number(value, width);
}

void fU_DOT() {
	uint value = static_cast<uint>(pop());
	print_unsigned_number(value);
}

void fDOT_R() {
	int width = pop();
	int value = pop();
	print_number(value, width);
}

void fU_DOT_R() {
	int width = pop();
	uint value = static_cast<uint>(pop());
	print_unsigned_number(value, width);
}

void fDEPTH() {
	int depth = vm.stack->depth();
    push(depth);	
}

void fRDEPTH() {
	int depth = vm.rstack->depth();
	push(depth);
}

void fSDEPTH() {
	int depth = vm.sstack->depth();
	push(depth);
}





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

void fPAD() {
	char* pad = vm.pad->pad();
	push(mem_addr(pad));
}

void fCOMMA() {
	int value = pop();
	comma(value);
}

void fC_COMMA() {
	int value = pop();
	ccomma(value);
}

void fALIGN() {
	align();
}

void fFIND() {
	int addr = pop();
	CountedString* word = reinterpret_cast<CountedString*>(mem_char_ptr(addr));
	Header* header = vm.dict->find_word(word->str(), word->size());
	if (header == nullptr) {
		push(addr);
		push(0);
	}
	else {
		int xt = header->xt();
		int is_immediate = header->flags.immediate ? F_TRUE : F_FALSE;
		if (is_immediate) {
			push(xt);
			push(1);
		}
		else {
			push(xt);
			push(-1);
		}
	}
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

void fPLUS() {
	int b = pop();
	int a = pop();
	push(a + b);
}

void fCOUNT() {
	int addr = pop();
	int len = cfetch(addr++);
	push(addr);
	push(len);
}

void fWORD() {
	char delimiter = pop();
	const ForthString* word = parse_word(delimiter);
	if (word == nullptr)
		exit(EXIT_SUCCESS);		        // no more input
	else {
		push(mem_addr(word->str()));	// address of word
		push(word->size());				// length of word
	}
}

void fS_QUOTE() {
	const ForthString* word = parse_word('"');
	if (word == nullptr)
		exit(EXIT_SUCCESS);		        // no more input
	else {
		push(mem_addr(word->str()));	// address of word
		push(word->size());				// length of word
	}
}

void fDROP() {
	pop();
}

void fDUP() {
	int value = peek(0);
	push(value);
}

void fPICK() {
	int depth = pop();
	if (depth < 0) {
		error(Error::InvalidMemoryAddress);
		return;
	}
	int value = peek(depth);
	push(value);
}

void fDOT_S() {
	vm.stack->print();
}

