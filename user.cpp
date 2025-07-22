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
	//@@END
}

//@@BEGIN: VarsImplementation
void fBASE() { push(vm.mem.addr(&vm.user->BASE)); }
void fSTATE() { push(vm.mem.addr(&vm.user->STATE)); }
//@@END

