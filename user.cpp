//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "user.h"
#include "vm.h"
using namespace std;

void User::init() {
	//@@BEGIN: VarsInit
	BASE = 10;
	STATE = STATE_INTERPRET;
	DPL = 0;
	TRACE = 0;
	//@@END
}

void fPAD() {
	char* pad = vm.pad->pad();
	push(vm.mem.addr(pad));
}

//@@BEGIN: VarsImplementation
void fTRUE() { push(F_TRUE); } // TRUE
void fFALSE() { push(F_FALSE); } // FALSE
void fBASE() { push(vm.mem.addr(&vm.user->BASE)); } // BASE
void fSTATE() { push(vm.mem.addr(&vm.user->STATE)); } // STATE
void fDPL() { push(vm.mem.addr(&vm.user->DPL)); } // DPL
void fTRACE() { push(vm.mem.addr(&vm.user->TRACE)); } // TRACE
//@@END

