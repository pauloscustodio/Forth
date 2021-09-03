//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"

// code words idWORD
typedef enum Code {
#define CODE(word, name, flags, value)	id##name,
#include "words.def"
} Code;

#define STATE_INTERPRET     0
#define STATE_COMPILE       1

// xt addresses: int xtWORD
#define CONST(word, name, flags, value)	extern int xt##name;
#define VAR(word, name, flags, value)	extern int xt##name;
#define CODE(word, name, flags, value)	extern int xt##name;
#define FORTH(word, name, flags, text)	extern int xt##name;
#include "words.def"

// interpreter pointer
extern int ip;

void c_interpret_word(const char* word, int len);
void c_evaluate(const char* text, int len);
void c_evaluate_sz(const char* text);
void f_evaluate(void);
void f_load(int blk);
void f_interpret(void);
void compile(const char* text);

void run_forth(void);
void f_execute(int xt);

void f_abort_quote(void);
void f_xabort_quote(void);

