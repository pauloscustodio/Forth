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
#include "locals.h"
#include "memory.h"
#include "output.h"
#include "stack.h"
#include "strings.h"
#include <set>
#include <string>
#include <unordered_map>

struct VM {
    VM();
    virtual ~VM();

    // instruction pointer
    int ip{ 0 };

    // abort error message
    std::string error_message;

    // word buffer
    char* wordbuf_data{ nullptr };
    uint wordbuf_ptr{ 0 };
    Wordbuf wordbuf;

    // pad
    char* pad_data{ nullptr };
    Pad pad;

    // number output buffer
    char* number_output_data{ nullptr };
    uint number_output_ptr{ 0 };
    uint precision{ 7 };    // number of significant digits for float output
    NumberOutput number_output;

    // input buffer
    char* tib_data{ nullptr };
    const char* tib_ptr{ nullptr };   // tib_data or string being EVALUATE'd
    Input input;			// input buffer

    // buffered block I/O
    char* block_data{ nullptr };
    Blocks blocks;

    // locals
    Locals locals;

    // memory
    Mem mem;

    // user variables
    User* user;

    // data stack
    Stack<int> stack{ '\0', Error::StackUnderflow };

    // return stack
    Stack<int> r_stack{ 'R', Error::ReturnStackUnderflow };

    // control stack
    Stack<int> cs_stack{ 'C', Error::ControlFlowStackUnderflow };

    // exception stack
    Stack<int> except_stack{ 'E', Error::ExceptionStackUnderflow };

    // floating point stack
    Stack<double> f_stack{ 'F', Error::FloatStackUnderflow };

    // condititional execution
    std::vector<bool> skipping_stack;   // stack of skipping states
    bool skipping{ false };             // currently skipping

    // dictionary
    uint dict_lo_mem, dict_hi_mem;      // memory limits for dictionary
    uint heap_lo_mem, heap_hi_mem;      // memory limits for heap

    uint latest_word;                   // last defined word
    // each wordlist latest word indexed by wid
    std::vector<uint> wordlists;
    std::vector<uint> search_order;     // search order of wordlists
    uint definitions_wid;               // wid where definitions are stored

    uint here;			// point to next free position at bottom of memory
    uint names;			// point to last name created at top of memory

    // list of substitutions
    std::unordered_map<std::string, std::string> substitutions;

    Dict dict;
    Heap heap;

    // files
    Files files;            // system files

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
double ffetch(uint addr);
void fstore(uint addr, double value);
double sffetch(uint addr);
void sfstore(uint addr, double value);
int cfetch(uint addr);
void cstore(uint addr, int value);

// allot dictionary space
void ccomma(int value);
void comma(int value);
void dcomma(dint value);
void fcomma(double value);
void align();

// stacks
void push(int value);
int pop();
int peek(uint depth = 0);
uint depth();
void roll(uint depth);

void dpush(dint value);
dint dpop();
dint dpeek(uint depth = 0);

void r_push(int value);
int r_pop();
int r_peek(uint depth = 0);
uint r_depth();

void r_dpush(dint value);
dint r_dpop();
dint r_dpeek(uint depth = 0);

enum {
    POS_COLON_START, POS_BEGIN_START, POS_DO_START, POS_CASE_START,

    POS_BEGIN_BACK, POS_DO_BACK,

    POS_IF_FWD, POS_ELSE_FWD, POS_WHILE_FWD,
    POS_DO_FWD, POS_LEAVE_FWD,
    POS_OF_FWD, POS_ENDOF_FWD,
};

void cs_dpush(dint pos_addr);
dint cs_dpop();
dint cs_dpeek(uint depth = 0);
void cs_droll(uint depth);
uint cs_ddepth();

void fpush(double value);
double fpop();
double fpeek(uint depth = 0);
uint fdepth();

void init_conditional();
void end_conditional();
void compute_skipping();
