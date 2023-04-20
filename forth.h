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
#define DICT_SZ         (256 * 1024)
#define STACK_SZ        1024
#define PAD_SZ          256

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

#define BL              ' '
#define CR              '\n'
#define SCREEN_WIDTH    80

// flags for name length
#define F_COUNTED_STR_MASK  0xff
#define F_LENMASK           0x3f
#define F_HIDDEN            0x40
#define F_IMMEDIATE         0x80

// compile state
#define STATE_INTERPRET     0
#define STATE_COMPILE       1

// VM 
typedef struct vm {
	char	pad[PAD_SZ];

#define VAR(word, name, flags, value)	int name;
#include "words.def"

	byte*	dict_p;
	byte*	latest_p;
	int 	sp;
	int 	rp;

	byte	dict[DICT_SZ];
	int		stack[STACK_SZ];
	int		rstack[STACK_SZ];
} VM;

extern VM vm;

void vm_init(void);

// memory
uint  ptr_to_addr(void *p);
char* addr_to_cptr(uint addr);
int*  addr_to_ptr(uint addr);

int   fetch(uint addr);
void  store(uint addr, int value);
dint  dfetch(uint addr);
void  dstore(uint addr, dint value);
int   cfetch(uint addr);
void  cstore(uint addr, int value);

void f_fill(void);
void f_erase(void);
void f_move(void);

uint counted_str_size(uint len);
const char* counted_str_fetch(uint addr, uint* len);
void counted_str_store(uint addr, const char* text, uint len);

uint dict_name_size(uint len);
const char* dict_name_fetch(uint addr, uint* len);
void dict_name_store(uint addr, const char* text, uint len, int flags);

int long_str_size(uint len);
const char* long_str_fetch(uint addr, uint* len);
void long_str_store(uint addr, const char* text, uint len);

// dictionary
uint here(void);
uint latest(void);
void align(void);
void comma(int value);
void dcomma(dint value);
void ccomma(int value);
void counted_str_comma(const char* text, uint len);
void dict_name_comma(const char* text, uint len, int flags);
void long_str_comma(const char* text, uint len);

// stacks
void push(int value);
int  pop(void);
int  peek(int depth);
int  depth(void);
void roll(int depth);
uint s0(void);
uint get_sp(void);
void set_sp(uint addr);
void print_stack();

void dpush(dint value);
dint dpop(void);
dint dpeek(int depth);

void rpush(int value);
int  rpop(void);
int  rpeek(int depth);
void rdpush(dint value);
dint rdpop(void);
dint rdpeek(int depth);
uint r0(void);
uint get_rp(void);
void set_rp(uint addr);
void print_rstack();

