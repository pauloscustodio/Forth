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

#define BYTE_SZ         ((int)sizeof(byte))
#define CELL_SZ         ((int)sizeof(int))
#define DCELL_SZ        ((int)sizeof(dint))

#define ALIGN(x)        (((x) + CELL_SZ - 1) & ~(CELL_SZ - 1))
#define DCELL_LO(x)     ((x) & 0xffffffffLL)
#define DCELL_HI(x)     (((x) >> 32) & 0xffffffffLL)
#define DCELL(hi, lo)   (((udint)DCELL_LO(hi) << 32) | (udint)DCELL_LO(lo))

#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define MIN(a, b)       ((a) < (b) ? (a) : (b))

#define F_TRUE          -1
#define F_FALSE          0
#define F_BOOL(x)       ((x) ? F_TRUE : F_FALSE)

// VM 
typedef struct vm {
	int		stack[STACK_SZ];
	int		sp;
	int		rstack[STACK_SZ];
	int		rp;
	int		base;
	byte	mem[MEM_SZ];
} VM;

extern VM vm;

void vm_init(void);

// stacks
void stacks_init(void);
void push(int value);
int  pop(void);
int  peek(int depth);
int  depth(void);
void roll(int depth);
void set_sp(int sp);
void print_stack();
void dpush(dint value);
dint dpop(void);
dint dpeek(int depth);
