//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "stack.h"
#include "errors.h"
#include "io.h"
#include <stdio.h>

void init_stacks(void) {
    user->sp = S0;
    user->rp = R0;
    user->struct_p = STRUCT0;
}

static void _push(int value, int* sp, int s1, ErrorCode overflow) {
    if (*sp - CELL_SZ <= s1)
        error(overflow);
    *sp -= CELL_SZ;
    store(*sp, value);
}

static int _pop(int* sp, int s0, ErrorCode underflow) {
    if (*sp >= s0)
        error(underflow);
    int out = fetch(*sp);
    *sp += CELL_SZ;
    return out;
}

static int _peek(int depth, int* sp, int s0, ErrorCode underflow) {
    int idx = *sp + depth * CELL_SZ;
    if (idx >= s0)
        error(underflow);
    return fetch(idx);
}

static void _dpush(dint value, int* sp, int s1, ErrorCode overflow) {
    _push(DCELL_LO(value), sp, s1, overflow);
    _push(DCELL_HI(value), sp, s1, overflow);
}

static dint _dpop(int* sp, int s0, ErrorCode underflow) {
    int hi = _pop(sp, s0, underflow);
    int lo = _pop(sp, s0, underflow);
    return DCELL(hi, lo);
}

static dint _dpeek(int depth, int* sp, int s0, ErrorCode underflow) {
    int hi = _peek(2 * depth, sp, s0, underflow);
    int lo = _peek(2 * depth + 1, sp, s0, underflow);
    return DCELL(hi, lo);
}

static int _depth(int* sp, int s0) {
    return (s0 - *sp) / CELL_SZ;
}

static void _print(int* sp, int s0, const char* prefix) {
    if (prefix)
        printf("(%s: ", prefix);
    else
        printf("( ");
    for (int p = s0 - CELL_SZ; p >= *sp; p -= CELL_SZ)
        print_int(fetch(p));
    printf(")");
}


void push(int value) {
    _push(value, &user->sp, S1, ErrorDataStackOveflow);
}

int pop(void) {
    return _pop(&user->sp, S0, ErrorDataStackUnderflow);
}

int peek(int depth) {
    return _peek(depth, &user->sp, S0, ErrorDataStackUnderflow);
}

void dpush(dint value) {
    _dpush(value, &user->sp, S1, ErrorDataStackOveflow);
}

dint dpop(void) {
    return _dpop(&user->sp, S0, ErrorDataStackUnderflow);
}

dint dpeek(int depth) {
    return _dpeek(depth, &user->sp, S0, ErrorDataStackUnderflow);
}

int depth(void) {
    return _depth(&user->sp, S0);
}

// e.g. roll(1)
// | 1 | 2 |    --> | 2 | 1 |
// ^sp ^bot
void roll(int depth) {
    if (depth > 0) {
        int bot = user->sp + depth * CELL_SZ;
        if (bot >= S0)
            error(ErrorDataStackUnderflow);
        int bot_value = fetch(bot);
        memmove(mem + user->sp + CELL_SZ, mem + user->sp, depth * CELL_SZ);
        store(user->sp, bot_value);
    }
}

void print_stack(void) {
    _print(&user->sp, S0, NULL);
}


void rpush(int value) {
    _push(value, &user->rp, R1, ErrorReturnStackOveflow);
}

int rpop(void) {
    return _pop(&user->rp, R0, ErrorReturnStackUnderflow);
}

int rpeek(int depth) {
    return _peek(depth, &user->rp, R0, ErrorReturnStackUnderflow);
}

void rdpush(dint value) {
    _dpush(value, &user->rp, R1, ErrorReturnStackOveflow);
}

dint rdpop(void) {
    return _dpop(&user->rp, R0, ErrorReturnStackUnderflow);
}

dint rdpeek(int depth) {
    return _dpeek(depth, &user->rp, R0, ErrorReturnStackUnderflow);
}

int rdepth(void) {
    return _depth(&user->rp, R0);
}

void print_rstack(void) {
    _print(&user->rp, R0, "R");
}


void struct_push(int value) {
    _push(value, &user->struct_p, STRUCT1, ErrorStructStackOveflow);
}

int struct_pop(void) {
    return _pop(&user->struct_p, STRUCT0, ErrorStructStackUnderflow);
}

int struct_peek(int depth) {
    return _peek(depth, &user->struct_p, STRUCT0, ErrorStructStackUnderflow);
}
