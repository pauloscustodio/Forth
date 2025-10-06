//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "environment.h"
#include "errors.h"
#include "forth.h"
#include "interp.h"
#include "locals.h"
#include "output.h"
#include "parser.h"
#include "tools.h"
#include "vm.h"

void f_interpret_word(const std::string& word) {
    f_interpret_word(word.c_str(), static_cast<uint>(word.size()));
}

void f_interpret_word(const char* word, uint size) {
    if (size > 0) {
        bool is_double = false;
        dint dvalue = 0;
        double fvalue = 0.0;
        VarName vname;

        Header* header = vm.dict.find_word(word, size);
        if (find_local(word, size, vname)) { // local found
            if (vm.user->STATE == STATE_INTERPRET) {
                error(Error::InterpretingACompileOnlyWord, std::string(word, word + size));
            }
            else {
                comma(xtXLITERAL);
                comma(vname.index);
                comma(xtXGET_LOCAL);
            }
        }
        else if (header) {	// word found
            uint xt = header->xt();
            if (header->flags.immediate || vm.user->STATE == STATE_INTERPRET) {
                f_execute(xt);
            }
            else {
                comma(xt);
            }
        }
        else if (parse_float(word, size, fvalue, true)) {
            if (vm.user->STATE == STATE_INTERPRET) {
                fpush(fvalue);

                if (vm.user->TRACE) {
                    std::cout << ">>" << BL << fvalue << BL;
                    vm.f_stack.print_debug();
                    std::cout << std::endl;
                }
            }
            else {
                comma(xtXFLITERAL);
                fcomma(fvalue);
            }
        }
        else if (parse_number(word, size, is_double, dvalue)) {
            if (is_double) { // double cell
                if (vm.user->STATE == STATE_INTERPRET) {
                    dpush(dvalue);

                    if (vm.user->TRACE) {
                        std::cout << ">>" << BL << dvalue << BL;
                        vm.stack.print_debug();
                        std::cout << std::endl;
                    }
                }
                else {
                    comma(xtX2LITERAL);
                    dcomma(dvalue);
                }
            }
            else { // single cell
                if (vm.user->STATE == STATE_INTERPRET) {
                    push(dcell_lo(dvalue));

                    if (vm.user->TRACE) {
                        std::cout << ">>" << BL << dcell_lo(dvalue) << BL;
                        vm.stack.print_debug();
                        std::cout << std::endl;
                    }
                }
                else {
                    comma(xtXLITERAL);
                    comma(dcell_lo(dvalue));
                }
            }
        }
        else {
            error(Error::UndefinedWord, std::string(word, word + size));
        }
    }
}

// implement [IF], [ELSE], [THEN] logic here
static void interpret_word(const char* word_ptr, uint size) {
    std::string word = to_upper(std::string(word_ptr, word_ptr + size));

    if (word == "[IF]") {
        bool flag = vm.skipping ? true : pop();
        vm.skipping_stack.push_back(!flag);
        compute_skipping();
    }
    else if (word == "[ELSE]") {
        // lone [ELSE] start a comment until [THEN]
        if (vm.skipping_stack.empty()) {
            vm.skipping_stack.push_back(true);
        }
        else {
            bool flag = vm.skipping_stack.back();
            vm.skipping_stack.back() = !flag;
        }
        compute_skipping();
    }
    else if (word == "[THEN]") {
        if (!vm.skipping_stack.empty()) {
            vm.skipping_stack.pop_back();
        }
        compute_skipping();
    }
    else if (vm.skipping) {
        // skip
    }
    else {
        f_interpret_word(word_ptr, size);
    }
}

void f_interpret() {
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);
        if (size) {
            interpret_word(word, size);
        }
        else if (vm.input.restore_input_if_query()) {
            continue;
        }
        else {
            break;
        }
    }

    if (vm.user->STATE == STATE_INTERPRET && g_interactive) {
        std::cout << BL << "ok" << std::endl;
    }
}

void f_evaluate() {
    uint size = pop();
    uint addr = pop();
    f_evaluate(mem_char_ptr(addr, size), size);
}

void f_evaluate(const std::string& text) {
    f_evaluate(text.c_str(), static_cast<uint>(text.size()));
}

void f_evaluate(const char* text, uint size) {
    // save input context
    vm.input.save_input();

    // parse string
    vm.user->BLK = 0;
    vm.input.set_text(text, size);
    f_execute(xtINTERPRET);

    // restore input context
    vm.input.restore_input();
}

void f_quit() {
    vm.r_stack.clear();
    vm.user->STATE = STATE_INTERPRET;
    init_conditional();
    while (true) {
        while (f_refill()) {
            f_interpret();
        }
        if (vm.input.restore_input()) {
            f_interpret();  // skip first refill(), buffer is already setup
            continue;
        }
        else {
            break;
        }
    }
    end_conditional();

    exit(EXIT_SUCCESS);
}

void enter_func(uint called_ip) {
    r_push(vm.ip);                  // return address
    vm.ip = called_ip;

    vm.locals.enter_frame();
}

void leave_func() {
    vm.ip = r_pop();                // recover return address

    vm.locals.leave_frame();
}

