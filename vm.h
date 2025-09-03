//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "block.h"
#include "dict.h"
#include "file.h"
#include "input.h"
#include "memory.h"
#include "output.h"
#include "stack.h"
#include "str.h"
#include <set>
using namespace std;

struct VM {
    VM();
    virtual ~VM();

    // instruction pointer
    int ip;

    // abort error message
    string* error_message;

    // memory
    Mem mem;

    // input
    Wordbuf* wordbuf;		// word buffer
    NumberOutput* number_output;	// HOLD buffer
    Pad* pad;				// pad
    Input* input;			// input buffer
    Blocks* blocks;			// buffered block I/O

    User* user;				// user variables

    // stacks
    Stack* r_stack;			// return stack
    Stack* cs_stack;		// control stack
    Stack* except_stack;	// exception stack
    Stack* stack;			// data stack

    // dictionary
    Dict* dict;

    // files
    Files* files;           // system files

    // included files
    std::set<std::string> included_files;
};

extern VM vm;

// pointer - address conversion
uint mem_addr(const char* ptr);
uint mem_addr(const int* ptr);
uint mem_addr(const uint* ptr);
uint mem_addr(const CString* ptr);

char* mem_char_ptr(uint addr, uint size = 0);
int* mem_int_ptr(uint addr, uint size = 0);

// access memory
int fetch(uint addr);
void store(uint addr, int value);
dint dfetch(uint addr);
void dstore(uint addr, dint value);
int cfetch(uint addr);
void cstore(uint addr, int value);

// allot dictionary space
void ccomma(int value);
void comma(int value);
void dcomma(dint value);
void align();

// stacks
void push(int value);
int pop();
int peek(int depth = 0);
int depth();
void roll(int depth);

void dpush(dint value);
dint dpop();
dint dpeek(int depth = 0);

void r_push(int value);
int r_pop();
int r_peek(int depth = 0);
int r_depth();

void r_dpush(dint value);
dint r_dpop();
dint r_dpeek(int depth = 0);

enum {
    POS_COLON_START, POS_BEGIN_START, POS_DO_START, POS_CASE_START,

    POS_BEGIN_BACK, POS_DO_BACK,

    POS_IF_FWD, POS_ELSE_FWD, POS_WHILE_FWD,
    POS_DO_FWD, POS_LEAVE_FWD,
    POS_OF_FWD, POS_ENDOF_FWD,
};

void cs_dpush(dint pos_addr);
dint cs_dpop();
dint cs_dpeek(int depth = 0);
int cs_ddepth();
