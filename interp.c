//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "interp.h"
#include "block.h"
#include "compiler.h"
#include "dict.h"
#include "double.h"
#include "errors.h"
#include "file.h"
#include "io.h"
#include "math.h"
#include "memory.h"
#include "parser.h"
#include "stack.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>

// xt addresses: int xtWORD
#define CONST(word, name, flags, value)	int xt##name;
#define VAR(word, name, flags, value)	int xt##name;
#define CODE(word, name, flags, value)	int xt##name;
#define FORTH(word, name, flags, text)	int xt##name;
#include "words.def"

// interpreter pointer
int ip;

// outer interpreter
void c_interpret_word(const char* word, int len) {
    if (len > 0) {
        int link = c_find_link(word, len);
        if (link) {		// word found
            int xt = link_to_xt(link);
            int flags = cfetch(link_to_name(link));
            if ((flags & F_IMMEDIATE) != 0 || user->STATE == STATE_INTERPRET)
                f_execute(xt);
            else
                comma(xt);
        }
        else {			// try a number
            dint value;
            int num_cells = c_parse_number(word, len, &value);
            switch (num_cells) {
            case 1:
                if (user->STATE == STATE_INTERPRET)
                    push(DCELL_LO(value));
                else {
                    comma(xtXLITERAL);
                    comma(DCELL_LO(value));
                }
                break;
            case 2:
                if (user->STATE == STATE_INTERPRET) {
                    push(DCELL_LO(value));
                    push(DCELL_HI(value));
                }
                else {
                    comma(xtXLITERAL);
                    comma(DCELL_LO(value));
                    comma(xtXLITERAL);
                    comma(DCELL_HI(value));
                }
                break;
            default:
                error_arg(ErrorWordNotDefined, fstr_to_cstr(word, len));
            }
        }
    }
}

void c_evaluate(const char* text, int len) {
    int old_source_id = user->source_id;    // save in case it's -1
    int old_blk = user->blk;
    user->blk = 0;

    push_text(text, len);
    f_execute(xtINTERPRET);
    pop_input();

    user->source_id = old_source_id;
    user->blk = old_blk;
}

void c_evaluate_sz(const char* text) {
    c_evaluate(text, strlen(text));
}

void f_evaluate(void) {
    int len = pop();
    int addr = pop();
    c_evaluate(mem + addr, len);
}

void f_load(int blk) {
    user->blk = blk;
    f_block(blk);

    f_execute(xtINTERPRET);

    user->blk = 0;
}

void f_interpret(void) {
    Buffer* buff;
    for (buff = cur_buffer(); buff->to_in < buff->nr_in; buff = cur_buffer()) {
        int len = 0;
        const char* word = c_parse_word(&len);		// parse next word
        c_interpret_word(word, len);
    }
    if (user->STATE == STATE_INTERPRET && interactive)
        printf(" ok\n");
}

// simple compiler used to compile code in words.def
void compile(const char* text) {
    int old_state = user->STATE;
    user->STATE = STATE_COMPILE;
    c_evaluate_sz(text);
    user->STATE = old_state;
}

void run_forth(void) {
    f_execute(xtQUIT);
}

// inner interpreter
void f_execute(int xt) {
    bool do_exit = false;
    int old_ip = ip;
    ip = 0;
    while (true) {
        Code code = (Code)fetch(xt);
        int body = xt + CELL_SZ;			// point to data area, if any

        switch (code) {
#define CODE(word, name, flags, c_code)	case id##name: { c_code; break; }
#include "words.def"

        default:
            error(ErrorInvalidForthCode);
        }

        if (ip == 0 || do_exit)			        // ip did not change, exit
            break;
        xt = fetch(ip);
        ip += CELL_SZ;	    // else fetch next xt from ip
    }
    ip = old_ip;
}

void f_abort_quote(void) {
    int len = 0;
    const char* message = c_parse('"', &len);
    comma(xtXABORT_QUOTE);
    long_str_comma(message, len);
}

void f_xabort_quote(void) {
    int len = 0;
    const char* message = long_str_fetch(ip, &len);
    ip += long_str_size(len);

    int f = pop();
    if (f) {
        fprintf(stderr, "\nAborted: %.*s\n", len, message);
        exit(EXIT_FAILURE);
    }
}
