//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "parser.h"
#include "vm.h"
#include <iostream>
#include <string>
#include <exception>

class ThrowException : public std::exception {
public:
    ThrowException(uint code) : error_code(code) {}
    virtual const char* what() const noexcept override {
        static std::string message;
        message = std::string("Forth exception thrown with error code ") +
                  std::to_string(error_code) + ": " + vm.error_message;
        return message.c_str();
    }

public:
    int error_code;
};

[[noreturn]] static void output_error(const std::string& message,
                                      const std::string& arg = "") {
    std::cerr << std::endl << "Error: " << message;
    if (!arg.empty()) {
        std::cerr << ": " << arg;
    }
    std::cerr << std::endl;
    exit(EXIT_FAILURE);
}

[[noreturn]] static void exit_error(Error err, const std::string& arg = "") {
    if (err == Error::None) {
        exit(EXIT_SUCCESS);
    }
    else if (err == Error::Abort) {
        exit(EXIT_FAILURE);
    }
    else if (err == Error::AbortQuote) {
        std::cerr << std::endl << "Aborted: " << vm.error_message << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        switch (err) {
#define X(code, id, message) case Error::id: output_error(message, arg); break;
#include "errors.def"
        default:
            output_error(std::to_string(static_cast<int>(err)));
            break;
        }
    }
}

[[noreturn]] static void exit_error(int error_code,
                                    const std::string& arg = "") {
    exit_error(static_cast<Error>(error_code), arg);
}

void error(Error err, const std::string& arg) {
    vm.error_message = arg;
    f_throw(err);
}

void f_catch() {
    uint xt = pop();
    f_catch(xt);
}

void f_catch(uint xt) {
    vm.except_stack.push(vm.r_stack.sp());
    vm.except_stack.push(vm.stack.sp());
    vm.except_stack.push(vm.input.input_level());
    vm.except_stack.push(vm.ip);

    int catch_result = 0;
    try {
        f_execute(xt);
    }
    catch (ThrowException& e) {
        vm.ip = vm.except_stack.pop(); // restore instruction pointer
        vm.input.restore_input(vm.except_stack.pop());
        vm.stack.set_sp(vm.except_stack.pop()); // restore data stack pointer
        vm.r_stack.set_sp(vm.except_stack.pop()); // restore return stack pointer

        catch_result = e.error_code;
    }
    catch (...) {
        throw;
    }
    push(catch_result);
}

void f_throw() {
    int error_code = pop();
    f_throw(error_code);
}

void f_throw(Error err) {
    f_throw(static_cast<int>(err));
}

void f_throw(int error_code) {
    if (error_code == 0) {
        return;
    }

    if (vm.except_stack.depth() == 0) {
        exit_error(error_code, vm.error_message);
    }
    else {
        throw ThrowException(error_code);
    }
}

void f_abort() {
    vm.stack.clear();
    vm.error_message.clear();
    f_throw(Error::Abort);
}

void f_abort_quote() {
    uint size;
    const char* message = parse_word(size, '"');
    if (vm.user->STATE == STATE_COMPILE) {
        int str_addr = vm.dict->alloc_string(message, size);
        comma(xtXABORT_QUOTE);
        comma(str_addr);
    }
    else {
        int error_code = pop();
        if (error_code != 0) {
            vm.stack.clear();
            vm.error_message = std::string(message, size);
            f_throw(Error::AbortQuote);
        }
    }
}

void f_xabort_quote() {
    int str_addr = fetch(vm.ip);
    vm.ip += CELL_SZ;

    int error_code = pop();
    if (error_code != 0) {
        LongString* str = reinterpret_cast<LongString*>(mem_char_ptr(str_addr));
        vm.stack.clear();
        vm.error_message = str->to_string();
        f_throw(Error::AbortQuote);
    }
}
