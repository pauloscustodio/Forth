//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "file.h"
#include "io.h"
#include "stack.h"

void f_included() {
	int len = pop();
	int filename = pop();
	push_file(mem + filename, len);
}
