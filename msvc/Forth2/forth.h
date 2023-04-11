//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

// VM size
#define MEM_SZ          (256 * 1024)
#define STACK_SZ        (16 * 1024)

// types
typedef uint8_t			byte;
typedef unsigned int	uint;
typedef int64_t			dint;
typedef uint64_t		udint;

// VM 
typedef struct vm {
	int		stack[STACK_SZ];
	int		sp;
	int		rstack[STACK_SZ];
	int		rp;
	byte	mem[MEM_SZ];
} VM;

extern VM vm;

