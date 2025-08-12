//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "control.h"
#include "dict.h"
#include "errors.h"
#include "vm.h"
#include <vector>
using namespace std;

void f_colon() {
    if (cs_ddepth() > 0)
        error(Error::CompilerNesting);

    cs_dpush(dcell(POS_COLON_START, 0));
    vm.dict->parse_create(idXDOCOL, F_SMUDGE);
    vm.user->STATE = STATE_COMPILE;

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

void f_colon_noname() {
    if (cs_ddepth() > 0)
        error(Error::CompilerNesting);

    cs_dpush(dcell(POS_COLON_START, 0));
    vm.dict->create("", F_SMUDGE, idXDOCOL);
    Header* header = reinterpret_cast<Header*>(
        mem_char_ptr(vm.dict->latest()));
    vm.user->STATE = STATE_COMPILE;
    push(header->xt());

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

void f_semicolon() {
    if (cs_ddepth() != 1)
        error(Error::CompilerNesting);

    dint pos_addr = cs_dpeek();
    if (dcell_hi(pos_addr) != POS_COLON_START)
        error(Error::CompilerNesting);
    cs_dpop();

    comma(xtXSEMICOLON);
    Header* header = reinterpret_cast<Header*>(
        mem_char_ptr(vm.dict->latest()));
    header->flags.smudge = false;
    vm.user->STATE = STATE_INTERPRET;

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

void f_recurse() {
    Header* header = reinterpret_cast<Header*>(
        mem_char_ptr(vm.dict->latest()));
    comma(header->xt());
}

static void comma_fwd_jump(int xt_jump, int pos) {
    if (pos != POS_IF_FWD &&
        pos != POS_ELSE_FWD &&
        pos != POS_WHILE_FWD &&
        pos != POS_DO_FWD &&
        pos != POS_LEAVE_FWD &&
        pos != POS_OF_FWD &&
        pos != POS_ENDOF_FWD)
        error(Error::CompilerNesting);

    comma(xt_jump);
    cs_dpush(dcell(pos, vm.dict->here()));
    comma(0);

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

static void resolve_fwd_jump() {
    if (cs_ddepth() < 1)
        error(Error::CompilerNesting);

    dint pos_patch = cs_dpeek();
    if (dcell_hi(pos_patch) != POS_IF_FWD &&
        dcell_hi(pos_patch) != POS_ELSE_FWD &&
        dcell_hi(pos_patch) != POS_WHILE_FWD &&
        dcell_hi(pos_patch) != POS_DO_FWD &&
        dcell_hi(pos_patch) != POS_LEAVE_FWD &&
        dcell_hi(pos_patch) != POS_OF_FWD &&
        dcell_hi(pos_patch) != POS_ENDOF_FWD)
        error(Error::CompilerNesting);
    cs_dpop();

    int dist = vm.dict->here() - dcell_lo(pos_patch);
    store(dcell_lo(pos_patch), dist);

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

static void mark_target_back_jump(int pos) {
    if (pos != POS_BEGIN_BACK &&
        pos != POS_DO_BACK)
        error(Error::CompilerNesting);

    int addr = vm.dict->here();
    cs_dpush(dcell(pos, addr));

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

static void resolve_back_jump(int xt_jump) {
    if (cs_ddepth() < 1)
        error(Error::CompilerNesting);

    dint pos_patch = cs_dpeek();
    if (dcell_hi(pos_patch) != POS_BEGIN_BACK &&
        dcell_hi(pos_patch) != POS_DO_BACK)
        error(Error::CompilerNesting);
    cs_dpop();

    comma(xt_jump);
    int dist = dcell_lo(pos_patch) - vm.dict->here();
    comma(dist);
}

static bool search_resolve_fwd_jump(int pos1, int pos2 = -1, int pos3 = -1) {
    vector<dint> save;
    bool resolved = false;
    while (cs_ddepth() > 0) {
        dint pos_target = cs_dpeek();
        if (dcell_hi(pos_target) == pos1 ||
            dcell_hi(pos_target) == pos2 ||
            dcell_hi(pos_target) == pos3) {
            resolve_fwd_jump();
            resolved = true;
            break;
        }
        else {
            save.push_back(cs_dpop());
        }
    }
    while (!save.empty()) {
        cs_dpush(save.back());
        save.pop_back();
    }

    if (vm.user->TRACE)
        vm.cs_stack->print();

    return resolved;
}

static bool resolve_all_fwd_jumps(int stop, 
    int pos1, int pos2 = -1, int pos3 = -1) {
    bool resolved = false;
    while (cs_ddepth() > 0) {
        dint pos_target = cs_dpeek();
        if (dcell_hi(pos_target) == stop) {
            cs_dpop();      // found the end
            break;
        }
        else if (dcell_hi(pos_target) == pos1 ||
            dcell_hi(pos_target) == pos2 ||
            dcell_hi(pos_target) == pos3) {
            resolve_fwd_jump();
            resolved = true;
        }
        else {
            resolved = false;   // other jumps unresolved 
            break;
        }
    }

    if (vm.user->TRACE)
        vm.cs_stack->print();
    
    return resolved;
}

static bool search_resolve_back_jump(int jump_xt, 
    int pos1, int pos2 = -1, int pos3 = -1) {
    vector<dint> save;
    bool resolved = false;
    while (cs_ddepth() > 0) {
        dint pos_target = cs_dpeek();
        if (dcell_hi(pos_target) == pos1 ||
            dcell_hi(pos_target) == pos2 ||
            dcell_hi(pos_target) == pos3) {
            resolve_back_jump(jump_xt);
            resolved = true;
            break;
        }
        else {
            save.push_back(cs_dpop());
        }
    }
    while (!save.empty()) {
        cs_dpush(save.back());
        save.pop_back();
    }

    if (vm.user->TRACE)
        vm.cs_stack->print();

    return resolved;
}

void f_if() {
    comma_fwd_jump(xtZBRANCH, POS_IF_FWD);
}

void f_else() {
    // make forward jump to THEN
    comma_fwd_jump(xtBRANCH, POS_ELSE_FWD);

    // patch forward jump at IF or WHILE
    if (!search_resolve_fwd_jump(POS_IF_FWD, POS_WHILE_FWD))
        error(Error::CompilerNesting);
}

void f_then() {
    // patch IF or ELSE or WHILE
    if (!search_resolve_fwd_jump(POS_IF_FWD, POS_ELSE_FWD, POS_WHILE_FWD))
        error(Error::CompilerNesting);
}

void f_begin() {
    mark_target_back_jump(POS_BEGIN_BACK);
}

void f_again() {
    // resolve jump to BEGIN
    if (!search_resolve_back_jump(xtBRANCH, POS_BEGIN_BACK))
        error(Error::CompilerNesting);
}

void f_until() {
    // resolve jump to BEGIN
    if (!search_resolve_back_jump(xtZBRANCH, POS_BEGIN_BACK))
        error(Error::CompilerNesting);
}

void f_while() {
    comma_fwd_jump(xtZBRANCH, POS_WHILE_FWD);
}

void f_repeat() {
    // go back in the stack looking for BEGIN (there can be more than one WHILE)
    if (!search_resolve_back_jump(xtBRANCH, POS_BEGIN_BACK))
        error(Error::CompilerNesting);

    // fix WHILE
    if (!search_resolve_fwd_jump(POS_WHILE_FWD))
        error(Error::CompilerNesting);
}

void f_do() {
    cs_dpush(dcell(POS_DO_START, 0));  // mark start of loop
    comma_fwd_jump(xtXDO, POS_DO_FWD);
    mark_target_back_jump(POS_DO_BACK);
}

void f_xdo() {
    int start = pop();
    int limit = pop();
    r_push(limit);
    r_push(start);
    vm.ip += CELL_SZ;
}

void f_query_do() {
    cs_dpush(dcell(POS_DO_START, 0));  // mark start of loop
    comma_fwd_jump(xtXQUERY_DO, POS_DO_FWD);
    mark_target_back_jump(POS_DO_BACK);
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

static void f_loop_plus_loop(int xt_jump) {
    if (!search_resolve_back_jump(xt_jump, POS_DO_BACK))
        error(Error::CompilerNesting);

    if (!resolve_all_fwd_jumps(POS_DO_START, POS_DO_FWD, POS_LEAVE_FWD))
        error(Error::CompilerNesting);
}

void f_loop() {
    f_loop_plus_loop(xtXLOOP);
}

void f_plus_loop() {
    f_loop_plus_loop(xtXPLUS_LOOP);
}

// ANS Forth expects +LOOP to check if the index crossed the boundary
static void f_xloop_step(int step) {
    int old_i = r_pop();
    int limit = r_pop();
    int new_i = old_i + step;
    int old_diff = old_i - limit;

    // crossing code lifted from pforth/Gforth
    // (x^y)<0 is equivalent to (x<0) != (y<0)
    bool crossed =
        (((old_diff ^ (old_diff + step))    // is the limit crossed?
            & (old_diff ^ step))            // is it a wrap-around?
            < 0);

    if (!crossed) {     // loop
        vm.ip += fetch(vm.ip);
        r_push(limit);
        r_push(new_i);
    }
    else {              // skip
        vm.ip += CELL_SZ;
    }
}

void f_xloop() {
    f_xloop_step(1);
}

void f_xplus_loop() {
    f_xloop_step(pop());
}

void f_leave() {
    comma_fwd_jump(xtXLEAVE, POS_LEAVE_FWD);
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

void f_case() {
    cs_dpush(dcell(POS_CASE_START, 0));    // mark start of case

    if (vm.user->TRACE)
        vm.cs_stack->print();
}

void f_of() {
    comma_fwd_jump(xtXOF, POS_OF_FWD);
}

void f_xof() {
    int b = pop();
    int a = pop();
    if (a != b) {
        push(a);                    // keep selector in stack
        vm.ip += fetch(vm.ip);
    }
    else
        vm.ip += CELL_SZ;           // drop selector and execute code
}

void f_endof() {
    comma_fwd_jump(xtBRANCH, POS_ENDOF_FWD);    // jump to ENDCASE

    if (!search_resolve_fwd_jump(POS_OF_FWD))
        error(Error::CompilerNesting);
}

void f_endcase() {
    comma(xtDROP);                  // discard selector

    // ignore the status as a CASE without OF has no fwd jumps
    resolve_all_fwd_jumps(POS_CASE_START, POS_ENDOF_FWD);
}
