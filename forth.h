//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define strncasecmp strnicmp
#define strcasecmp  stricmp
#endif

// VM size
#define MEM_SZ          (256 * 1024)
#define STACK_SZ        (1024)

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

void vm_init(void);

// stack
void stacks_init(void);
void push(int value);
int  pop(void);
int  peek(int depth);
void print_stack();
