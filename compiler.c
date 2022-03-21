//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2022
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------


#include "compiler.h"
#include "memory.h"
#include "dict.h"
#include "interp.h"
#include "stack.h"

static void comma_fwd_jump(int xt) {
    comma(xt);
    int ptr = user->here;
    comma(0);
    push(ptr);
}

static void resolve_fwd_jump() {
    int ptr = pop();
    int dist = user->here - ptr;
    store(ptr, dist);
}

static void mark_target_back_jump() {
    int ptr = user->here;
    push(ptr);
}

static void resolve_back_jump(int xt) {
    comma(xt);
    int ptr = pop();
    int dist = ptr - user->here;
    comma(dist);
}

void f_if(void) {
    comma_fwd_jump(xtZBRANCH);
}

void f_else(void) {
    int ptr = pop();
    comma_fwd_jump(xtBRANCH);
    push(ptr);
    resolve_fwd_jump();
}

void f_then(void) {
    resolve_fwd_jump();
}

void f_do(void) {
    comma_fwd_jump(xtXDO);
    mark_target_back_jump();
    struct_push(-1);      // mark start of loop
}

void f_xdo(void) {
    int start = pop();
    int limit = pop();
    rpush(limit);
    rpush(start);
    ip += CELL_SZ;
}

void f_query_do(void) {
    comma_fwd_jump(xtXQUERY_DO);
    mark_target_back_jump();
    struct_push(-1);      // mark start of loop
}

void f_xquery_do(void) {
    int start = pop();
    int limit = pop();
    if (start != limit) {
        rpush(limit);
        rpush(start);
        ip += CELL_SZ;
    }
    else
        ip += fetch(ip);
}

static void f_loop_plus_loop(int xt) {
    resolve_back_jump(xt);
    resolve_fwd_jump();
    while (struct_peek(0) != -1) {
        push(struct_pop());
        resolve_fwd_jump();
    }
    struct_pop();       // remove marker
}

void f_loop(void) {
    f_loop_plus_loop(xtXLOOP);
}

static void f_xloop_step(int step) {
    int i = rpop();
    int limit = rpop();
    i += step;
    bool repeat;
    if (step > 0)
        repeat = (i < limit);
    else
        repeat = (i >= limit);
    if (repeat) {
        ip += fetch(ip);
        rpush(limit);
        rpush(i);
    }
    else
        ip += CELL_SZ;
}

void f_xloop(void) {
    f_xloop_step(1);
}

void f_plus_loop(void) {
    f_loop_plus_loop(xtXPLUS_LOOP);
}

void f_xplus_loop(void) {
    f_xloop_step(pop());
}

void f_leave(void) {
    comma_fwd_jump(xtXLEAVE);
    struct_push(pop());   // move jump addr to struct_stack
}

void f_xleave(void) {
    f_xunloop();
    ip += fetch(ip);                // jump to end
}

void f_unloop(void) {
    comma(xtXUNLOOP);
}

void f_xunloop(void) {
    rpop();                         // drop I
    rpop();                         // drop limit
}

void f_begin(void) {
    mark_target_back_jump();
}

void f_again(void) {
    resolve_back_jump(xtBRANCH);
}

void f_until(void) {
    resolve_back_jump(xtZBRANCH);
}

void f_while(void) {
    comma_fwd_jump(xtZBRANCH);
}

void f_repeat(void) {
    int while_ptr = pop();
    int begin_ptr = pop();
    push(while_ptr);
    push(begin_ptr);
    resolve_back_jump(xtBRANCH);
    resolve_fwd_jump();
}

void f_case(void) {
    struct_push(-1);        // mark start of case
}

void f_of(void) {
    comma_fwd_jump(xtXOF);
}

void f_xof(void) {
    int b = pop();
    int a = pop();
    if (a != b) {
        push(a);            // keep selector in stack
        ip += fetch(ip);
    }
    else
        ip += CELL_SZ;      // drop selector and execute code
}

void f_endof(void) {
    comma_fwd_jump(xtBRANCH);
    struct_push(pop());     // move jump addr to struct_stack
    resolve_fwd_jump();
}

void f_endcase(void) {
    comma(xtDROP);          // discard selector
    while (struct_peek(0) != -1) {
        push(struct_pop());
        resolve_fwd_jump();
    }
    struct_pop();           // remove marker
}
