//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "environ.h"
#include "errors.h"
#include "interp.h"
#include "parser.h"
#include "vm.h"
using namespace std;

// interpreter pointer
int ip = 0;

void f_interpret_word(const string& word) {
    f_interpret_word(word.c_str(), word.size());
}

void f_interpret_word(const char* word, size_t size) {
    f_interpret_word(word, static_cast<int>(size));
}

void f_interpret_word(const char* word, int size) {
    if (size > 0) {
        bool is_double = false;
        dint value = 0;

        Header* header = vm.dict->find_word(word, size);
        if (header) {	// word found
            int xt = header->xt();
            if (header->flags.immediate || vm.user->STATE == STATE_INTERPRET) {
                if (vm.user->TRACE) {
                    ForthString* name = header->name();
                    cout << ">>" << BL << name->to_string() << BL;
                }

                f_execute(xt);

                if (vm.user->TRACE) {
                    vm.stack->print();
                    cout << endl;
                }
            }
            else {
                comma(xt);
            }
        }
        else if (f_parse_number(word, size, is_double, value)) {
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
                    comma(xtXLITERAL);
                    comma(dcell_lo(value));
                    comma(xtXLITERAL);
                    comma(dcell_hi(value));
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
        const ForthString* word = f_parse_word(BL);
        if (word == nullptr)
            break;
        f_interpret_word(word->str(), word->size());
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
    vm.input->push_text(text, size);
    f_execute(xtINTERPRET);
    vm.input->pop_input();
}

void f_quit() {
    vm.rstack->clear();
    vm.user->STATE = STATE_INTERPRET;
    while (f_refill())
        f_interpret();
    exit(EXIT_SUCCESS);
}
