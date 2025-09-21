//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "math.h"
#include "output.h"
#include "vm.h"
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

void NumberOutput::init() {
    memset(vm.number_output_data, BL, NUMBER_OUTPUT_SZ);
    start();
}

void NumberOutput::start() {
    vm.number_output_ptr = NUMBER_OUTPUT_SZ;
}

void NumberOutput::add_digit() {
    // compute digit
    udint value = dpop();

    // can use % instead of f_mod because value is assumed positive
    int digit = static_cast<int>(value % vm.user->BASE);

    value /= vm.user->BASE;
    dpush(value);

    // output it
    if (digit < 10) {
        add_char('0' + digit);
    }
    else {
        add_char('A' + digit - 10);
    }
}

void NumberOutput::add_digits() {
    dint value;
    do {
        add_digit();
        value = dpeek();
    }
    while (value != 0);
}

void NumberOutput::add_char(char c) {
    if (vm.number_output_ptr < 1) {
        error(Error::PicturedNumericOutputStringOverflow);
    }
    else {
        vm.number_output_data[--vm.number_output_ptr] = c;
    }
}

void NumberOutput::add_sign(int sign) {
    if (sign < 0) {
        add_char('-');
    }
}

void NumberOutput::add_string(const std::string& str) {
    add_string(str.c_str(), static_cast<uint>(str.size()));
}

void NumberOutput::add_string(const char* str, uint size) {
    for (int i = size - 1; i >= 0; --i) {
        add_char(str[i]);
    }
}

void NumberOutput::end() const {
    dpop();     // drop number
    push(mem_addr(vm.number_output_data + vm.number_output_ptr));
    push(NUMBER_OUTPUT_SZ - vm.number_output_ptr);
}

void NumberOutput::end_print() const {
    dpop();     // drop number
    const char* str = vm.number_output_data + vm.number_output_ptr;
    uint size = NUMBER_OUTPUT_SZ - vm.number_output_ptr;
    print_string(str, size);
}

static std::string print_dint_uint(int sign) {
    vm.number_output.start();
    vm.number_output.add_char(BL);
    vm.number_output.add_digits();
    vm.number_output.add_sign(sign);
    vm.number_output.end();
    uint size = pop();
    uint addr = pop();
    char* str = mem_char_ptr(addr, size);
    return std::string(str, str + size);
}

static std::string print_dint_uint_aligned(int width, int sign) {
    vm.number_output.start();

    dint d;
    do {
        vm.number_output.add_digit();
        width--;
        d = dpeek();
    }
    while (d != 0);

    if (sign < 0) {
        vm.number_output.add_sign(sign);
        width--;
    }

    while (width-- > 0) {
        vm.number_output.add_char(BL);
    }

    vm.number_output.end();
    uint size = pop();
    uint addr = pop();
    char* str = mem_char_ptr(addr, size);
    return std::string(str, str + size);
}

std::string char_to_string(char c) {
    std::ostringstream oss;
    oss << c;
    return oss.str();
}

void print_char(char c) {
    std::cout << char_to_string(c);
}

void print_string(const std::string& str) {
    for (char c : str) {
        print_char(c);
    }
}

std::string spaces_to_string(int count) {
    if (count < 1) {
        return "";
    }
    else {
        return std::string(count, BL);
    }
}

void print_spaces(int count) {
    print_string(spaces_to_string(count));
}

std::string string_to_string(uint addr, uint size) {
    return string_to_string(mem_char_ptr(addr, size), size);
}

std::string string_to_string(const char* str, uint size) {
    std::ostringstream oss;
    for (uint i = 0; i < size; ++i) {
        oss << str[i];
    }
    return oss.str();
}

void print_string(uint addr, uint size) {
    print_string(mem_char_ptr(addr, size), size);
}

void print_string(const char* str, uint size) {
    print_string(string_to_string(str, size));
}

std::string number_to_string(int value) {
    dpush(std::abs(static_cast<dint>(value)));
    int sign = value;
    return print_dint_uint(sign);
}

void print_number(int value) {
    print_string(number_to_string(value));
}

std::string number_to_string(dint value) {
    dpush(std::abs(value));
    int sign = value < 0 ? -1 : 1;
    return print_dint_uint(sign);
}

void print_number(dint value) {
    print_string(number_to_string(value));
}

std::string number_to_string(double value) {
    std::ostringstream oss;
    oss << std::setprecision(vm.precision - 1)
        << (std::fabs(value) < EPSILON ? 0 : value);
    return oss.str();
}

void print_number(double value) {
    print_string( number_to_string(value));
}

std::string number_fixed_to_string(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(vm.precision - 1)
        << (std::fabs(value) < EPSILON ? 0 : value);
    std::string number = oss.str();
    if (number.find('.') == std::string::npos) {
        number.push_back('.');
    }

    // Remove trailing zeros
    number.erase(number.find_last_not_of('0') + 1);

    number.push_back(BL);
    return number;
}

void print_number_fixed(double value) {
    print_string(number_fixed_to_string(value));
}

std::string number_engineering_to_string(double value) {
    if (std::fabs(value) < EPSILON) {
        return "0. ";
    }
    else {
        int exponent = static_cast<int>(std::floor(std::log10(std::fabs(value))));
        int eng_exponent = exponent >= 0
                           ? exponent - (exponent % 3)
                           : exponent - ((exponent % 3 + 3) % 3);
        double significand = value / std::pow(10.0, eng_exponent);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(vm.precision - 1)
            << significand;

        // Trim trailing zeros and decimal point
        std::string significand_str = oss.str();
        significand_str.erase(significand_str.find_last_not_of('0') + 1);
        if (significand_str.back() == '.') {
            significand_str.pop_back();
        }

        std::ostringstream result;
        result << significand_str << "E" << eng_exponent << BL;
        return result.str();
    }
}

void print_number_engineering(double value) {
    print_string(number_engineering_to_string(value));
}

std::string number_scientific_to_string(double value) {
    if (std::fabs(value) < EPSILON) {
        return "0E+0 ";
    }
    else {
        int exponent = static_cast<int>(std::floor(std::log10(std::fabs(value))));
        double significand = value / std::pow(10.0, exponent);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(vm.precision - 1)
            << significand;

        // Trim trailing zeros and decimal point
        std::string significand_str = oss.str();
        significand_str.erase(significand_str.find_last_not_of('0') + 1);
        if (significand_str.back() == '.') {
            significand_str.pop_back();
        }

        std::ostringstream result;
        result << significand_str << "E" << (exponent >= 0 ? "+" : "") << exponent <<
               " ";
        return result.str();
        return std::string();
    }
}

void print_number_scientific(double value) {
    print_string(number_scientific_to_string(value));
}

std::string number_dot_to_string(dint value) {
    std::string number = number_to_string(value, 0);
    number.push_back('.');
    number.push_back(BL);
    return number;
}

void print_number_dot(dint value) {
    print_string(number_dot_to_string(value));
}

std::string number_e_to_string(double value) {
    std::ostringstream oss;
    oss << std::setprecision(vm.precision - 1)
        << (std::fabs(value) < EPSILON ? 0 : value);
    std::string number = oss.str();
    if (number.find('e') == std::string::npos &&
            number.find('E') == std::string::npos) {
        number.push_back('e');
    }
    number.push_back(BL);
    return number;
}

void print_number_e(double value) {
    print_string(number_e_to_string(value));
}

std::string number_to_string(int value, int width) {
    dpush(std::abs(value));
    return print_dint_uint_aligned(width, value);
}

void print_number(int value, int width) {
    print_string(number_to_string(value, width));
}

std::string number_to_string(dint value, int width) {
    dpush(std::abs(value));
    int sign = value < 0 ? -1 : 1;
    return print_dint_uint_aligned(width, sign);
}

void print_number(dint value, int width) {
    print_string(number_to_string(value, width));
}

std::string unsigned_number_to_string(uint value) {
    push(value);    // lo
    push(0);        // hi
    return print_dint_uint(+1);
}

void print_unsigned_number(uint value) {
    print_string(unsigned_number_to_string(value));
}

std::string unsigned_number_to_string(uint value, int width) {
    push(value);
    push(0);
    return print_dint_uint_aligned(width, 1);
}

void print_unsigned_number(uint value, int width) {
    print_string(unsigned_number_to_string(value, width));
}
