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
#include "output.h"
#include "parser.h"
#include "vm.h"

void interpret_word(const std::string& word) {
    interpret_word(word.c_str(), static_cast<uint>(word.size()));
}

void interpret_word(const char* word, uint size) {
    if (size > 0) {
        bool is_double = false;
        dint dvalue = 0;
        double fvalue = 0.0;

        Header* header = vm.dict.find_word(word, size);
        if (header) {	// word found
            uint xt = header->xt();
            if (header->flags.immediate || vm.user->STATE == STATE_INTERPRET) {
                f_execute(xt);
            }
            else {
                comma(xt);
            }
        }
        else if (parse_float(word, size, fvalue)) {
            if (vm.user->STATE == STATE_INTERPRET) {
                f_push(fvalue);

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

void f_interpret() {
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);
        if (size == 0) {
            if (vm.input.restore_input_if_query()) {
                continue;
            }
            else {
                break;
            }
        }

        interpret_word(word, size);
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

    exit(EXIT_SUCCESS);
}

