//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "env.h"
#include "errors.h"
#include "forth.h"
#include "interp.h"
#include "output.h"
#include "parser.h"
#include "vm.h"
using namespace std;

void interpret_word(const string& word) {
    interpret_word(word.c_str(), word.size());
}

void interpret_word(const char* word, size_t size) {
    interpret_word(word, static_cast<int>(size));
}

void interpret_word(const char* word, int size) {
    if (size > 0) {
        bool is_double = false;
        dint value = 0;

        Header* header = vm.dict->find_word(word, size);
        if (header) {	// word found
            int xt = header->xt();
            if (header->flags.immediate || vm.user->STATE == STATE_INTERPRET) {
                f_execute(xt);
            }
            else {
                comma(xt);
            }
        }
        else if (parse_number(word, size, is_double, value)) {
            if (is_double) { // double cell
                if (vm.user->STATE == STATE_INTERPRET) {
                    dpush(value);

                    if (vm.user->TRACE) {
                        cout << ">>" << BL;
                        print_number(value);
                        cout << BL;
                        vm.stack->print();
                        cout << endl;
                    }
                }
                else {
                    comma(xtX2LITERAL);
                    dcomma(value);
                }
            }
            else { // single cell
                if (vm.user->STATE == STATE_INTERPRET) {
                    push(dcell_lo(value));

                    if (vm.user->TRACE) {
                        cout << ">>" << BL;
                        print_number(dcell_lo(value));
                        cout << BL;
                        vm.stack->print();
                        cout << endl;
                    }
                }
                else {
                    comma(xtXLITERAL);
                    comma(dcell_lo(value));
                }
            }
        }
        else {
            error(Error::UndefinedWord, string(word, word + size));
        }
    }
}

void f_interpret() {
    while (true) {
        int size;
        const char* word = parse_word(size, BL);
        if (size == 0) {
            if (vm.input->restore_input_if_query())
                continue;
            else
                break;
        }

        interpret_word(word, size);
    }

    if (vm.user->STATE == STATE_INTERPRET && g_interactive)
        cout << BL << "ok" << endl;
}

void f_evaluate() {
    int size = pop();
    int addr = pop();
    f_evaluate(mem_char_ptr(addr), size);
}

void f_evaluate(const string& text) {
    f_evaluate(text.c_str(), text.size());
}

void f_evaluate(const char* text, size_t size) {
    f_evaluate(text, static_cast<int>(size));
}

void f_evaluate(const char* text, int size) {
    // save input context
    vm.input->save_input();
    
    // parse string
    vm.user->BLK = 0;
    vm.input->set_text(text, size);
    f_execute(xtINTERPRET);

    // restore input context
    vm.input->restore_input();
}

void f_quit() {
    vm.r_stack->clear();
    vm.user->STATE = STATE_INTERPRET;
    while (true) {
        while (f_refill())
            f_interpret();
        if (vm.input->restore_input()) {
            f_interpret();  // skip first refill(), buffer is already setup
            continue;
        }
        else
            break;
    }
    
    exit(EXIT_SUCCESS);
}

void f_abort_quote() {
    int size;
    const char* message = parse_word(size, '"');
    int str_addr = vm.dict->alloc_string(message, size);
    comma(xtXABORT_QUOTE);
    comma(str_addr);
}

void f_xabort_quote() {
    int str_addr = fetch(vm.ip); vm.ip += CELL_SZ;
    const LongString* message = reinterpret_cast<const LongString*>(mem_char_ptr(str_addr));

    int f = pop();
    if (f) {
        cerr << endl << "Aborted: " << message->to_string() << endl;
        exit(EXIT_FAILURE);
    }
}

