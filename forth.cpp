//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

//@@BEGIN: WordsXtDefinition
int xtBASE = 0; // BASE
int xtSTATE = 0; // STATE
int xtDPL = 0; // DPL
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

void fENVIRONMENT_Q() {
	int size = pop();
	int addr = pop();
	const char* str = vm.mem.char_ptr(addr);
	string query{ str, str + size };
	if (query == "CORE") {
		push(F_TRUE);
		push(F_TRUE);
	}
	else {
		push(F_FALSE);
	}
}

