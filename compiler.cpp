//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "compiler.h"
#include "forth.h"
#include "vm.h"
using namespace std;

static void comma_fwd_jump(int xt) {
    comma(xt);
    int ptr = vm.dict->here();
    comma(0);
    push(ptr);
}

static void resolve_fwd_jump() {
    int ptr = pop();
    int dist = vm.dict->here() - ptr;
    store(ptr, dist);
}

static void mark_target_back_jump() {
    int ptr = vm.dict->here();
    push(ptr);
}

static void resolve_back_jump(int xt) {
    comma(xt);
    int ptr = pop();
    int dist = ptr - vm.dict->here();
    comma(dist);
}

void f_if() {
    comma_fwd_jump(xtZBRANCH);
}

void f_else() {
    int ptr = pop();
    comma_fwd_jump(xtBRANCH);
    push(ptr);
    resolve_fwd_jump();
}

void f_then() {
    resolve_fwd_jump();
}

void f_do() {
    comma_fwd_jump(xtXDO);
    mark_target_back_jump();
    cs_push(-1);      // mark start of loop
}

void f_xdo() {
    int start = pop();
    int limit = pop();
    r_push(limit);
    r_push(start);
    vm.ip += CELL_SZ;
}

void f_query_do() {
    comma_fwd_jump(xtXQUERY_DO);
    mark_target_back_jump();
    cs_push(-1);      // mark start of loop
}

void f_xquery_do() {
    int start = pop();
    int limit = pop();
    if (start != limit) {
        r_push(limit);
        r_push(start);
        vm.ip += CELL_SZ;
    }
    else
        vm.ip += fetch(vm.ip);
}

static void f_loop_plus_loop(int xt) {
    resolve_back_jump(xt);
    resolve_fwd_jump();
    while (cs_peek() != -1) {
        push(cs_pop());
        resolve_fwd_jump();
    }
    cs_pop();       // remove marker
}

void f_loop() {
    f_loop_plus_loop(xtXLOOP);
}

static void f_xloop_step(int step) {
    int i = r_pop();
    int limit = r_pop();
    i += step;
    bool repeat;
    if (step > 0)
        repeat = (i < limit);
    else
        repeat = (i >= limit);
    if (repeat) {
        vm.ip += fetch(vm.ip);
        r_push(limit);
        r_push(i);
    }
    else
        vm.ip += CELL_SZ;
}

void f_xloop() {
    f_xloop_step(1);
}

void f_plus_loop() {
    f_loop_plus_loop(xtXPLUS_LOOP);
}

void f_xplus_loop() {
    f_xloop_step(pop());
}

void f_leave() {
    comma_fwd_jump(xtXLEAVE);
    cs_push(pop());       // move jump addr to struct_stack
}

void f_xleave() {
    f_xunloop();
    vm.ip += fetch(vm.ip);                // jump to end
}

void f_unloop() {
    comma(xtXUNLOOP);
}

void f_xunloop() {
    r_pop();                        // drop I
    r_pop();                        // drop limit
}

void f_begin() {
    mark_target_back_jump();
}

void f_again() {
    resolve_back_jump(xtBRANCH);
}

void f_until() {
    resolve_back_jump(xtZBRANCH);
}

void f_while() {
    comma_fwd_jump(xtZBRANCH);
}

void f_repeat() {
    int while_ptr = pop();
    int begin_ptr = pop();
    push(while_ptr);
    push(begin_ptr);
    resolve_back_jump(xtBRANCH);
    resolve_fwd_jump();
}

void f_recurse() {
    Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
    comma(header->xt());
}

void f_case() {
    cs_push(-1);          // mark start of case
}

void f_of() {
    comma_fwd_jump(xtXOF);
}

void f_xof() {
    int b = pop();
    int a = pop();
    if (a != b) {
        push(a);                    // keep selector in stack
        vm.ip += fetch(vm.ip);
    }
    else
        vm.ip += CELL_SZ;              // drop selector and execute code
}

void f_endof() {
    comma_fwd_jump(xtBRANCH);
    cs_push(pop());       // move jump addr to struct_stack
    resolve_fwd_jump();
}

void f_endcase() {
    comma(xtDROP);                  // discard selector
    while (cs_peek() != -1) {
        push(cs_pop());
        resolve_fwd_jump();
    }
    cs_pop();             // remove marker
}
