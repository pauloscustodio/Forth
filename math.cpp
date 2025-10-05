//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "math.h"
#include "stack.h"
#include "vm.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>

int f_mod(int a, int b) {
    if (b == 0) {
        error(Error::DivisionByZero);
        return 0; // not reached
    }
    // Handle edge cases where negation does not change the value
    else if (b == std::numeric_limits<int>::min()) {
        // Only possible remainders are a or a + b, depending on sign
        int ret = a % b;
        if (ret < 0) {
            ret += b;
        }
        return ret;
    }
    else if (a == std::numeric_limits<int>::min() && b == -1) {
        // Avoid overflow
        return 0;
    }
    else if (b < 0) {
        return -f_mod(-a, -b);
    }
    else {
        int ret = a % b;
        if (ret < 0) {
            ret += b;
        }
        return ret;
    }
}

int f_div(int a, int b) {
    int rem = f_mod(a, b);
    int quot = (a - rem) / b;
    return quot;
}

double f_div(double a, double b) {
    return a / b;
}

void f_div_mod() {
    int b = pop();
    int a = pop();
    push(f_mod(a, b));
    push(f_div(a, b));
}

dint f_dmod(dint a, dint b) {
    if (b == 0) {
        error(Error::DivisionByZero);
        return 0;   // not reached
    }
    else if (b < 0) {
        return -f_dmod(-a, -b);
    }
    else {
        dint ret = a % b;
        if (ret < 0) {
            ret += b;
        }
        return ret;
    }
}

dint f_ddiv(dint a, dint b) {
    dint rem = f_dmod(a, b);
    dint quot = (a - rem) / b;
    return quot;
}

void f_ddiv_mod() {
    dint b = dpop();
    dint a = dpop();
    dpush(f_dmod(a, b));
    dpush(f_ddiv(a, b));
}

void f_fm_div_mod() {
    dint n = (dint)pop();
    dint d = dpop();
    push(static_cast<int>(f_dmod(d, n)));
    push(static_cast<int>(f_ddiv(d, n)));
}

void f_sm_div_rem() {
    dint n = static_cast<dint>(pop());
    dint d = dpop();
    if (n == 0) {
        error(Error::DivisionByZero);
    }
    else {
        push(static_cast<int>(d % n));
        push(static_cast<int>(d / n));
    }
}

void f_um_div_mod() {
    udint n = static_cast<uint>(pop());
    udint d = dpop();
    if (n == 0) {
        error(Error::DivisionByZero);
    }
    else {
        push(static_cast<int>(d % n));
        push(static_cast<int>(d / n));
    }
}

static void mul_div_mod(int a, int b, int c, int& quot, int& rem) {
    dint prod = (dint)a * (dint)b;
    quot = static_cast<int>(f_ddiv(prod, c));
    rem = static_cast<int>(f_dmod(prod, c));
}

void f_mul_div_mod() {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, quot, rem);
    push(rem);
    push(quot);
}

void f_mul_div() {
    int c = pop(), b = pop(), a = pop(), quot, rem;
    mul_div_mod(a, b, c, quot, rem);
    push(quot);
}

bool within(uint x, uint lo, uint hi) {
    // implement the same logic as Forth's WITHIN word
    return (x - lo) < (hi - lo);
}

void f_within() {
    uint hi = pop();
    uint lo = pop();
    uint x = pop();
    push(f_bool(within(x, lo, hi)));
}

void f_um_mult() {
    udint b = static_cast<uint>(pop());
    udint a = static_cast<uint>(pop());
    udint result = a * b;
    dpush(result);
}

void f_m_plus() {
    dint n = pop();
    dint d = dpop();
    dpush(d + n);
}

void d_to_f() {
    dint d = dpop();
    double f = static_cast<double>(d);
    fpush(f);
}

void f_to_d() {
    double f = fpop();
    dint d = static_cast<dint>(std::trunc(f));
    dpush(d);
}

void s_to_f() {
    int d = pop();
    double f = static_cast<double>(d);
    fpush(f);
}

void f_to_s() {
    double f = fpop();
    int d = static_cast<int>(std::trunc(f));
    push(d);
}

RepresentResult f_represent(double x, int significant_digits) {
    RepresentResult result;

    // Handle sign
    result.is_negative = std::signbit(x);
    double abs_x = std::fabs(x);

    // Use scientific notation to extract exponent and digits
    std::ostringstream oss;
    oss << std::scientific
        << std::setprecision(significant_digits - 1)
        << abs_x;
    std::string sci = oss.str();  // e.g., "1.234567890123456e+03"

    // Parse digits and exponent
    size_t e_pos = sci.find('e');
    std::string mantissa = sci.substr(0, e_pos);
    std::string exponent_str = sci.substr(e_pos + 1);

    // Remove decimal point from mantissa
    mantissa.erase(std::remove(mantissa.begin(), mantissa.end(), '.'),
                   mantissa.end());

    result.digits = mantissa;
    // Adjust exponent for leading digit
    result.exponent = std::stoi(exponent_str) + 1;

    return result;
}

void f_represent() {
    uint size = pop();
    uint addr = pop();
    char* buffer = mem_char_ptr(addr, size);

    double f = fpop();

    RepresentResult res = f_represent(f, size);

    memset(buffer, BL, size);
    memcpy(buffer, res.digits.c_str(),
           std::min(size, static_cast<uint>(res.digits.size())));

    push(res.exponent);
    push(res.is_negative ? F_TRUE : F_FALSE);
    push(F_TRUE);  // success
}

bool f_f_tilde() {
    double tolerance = fpop();
    double b = fpop();
    double a = fpop();
    return f_f_tilde(a, b, tolerance);
}

// Bitwise comparison for exact encoding (handles +0 vs -0)
static bool bitwise_equal(double a, double b) {
    static_assert(sizeof(double) == sizeof(uint64_t),
                  "Unexpected double size");
    uint64_t ua, ub;
    std::memcpy(&ua, &a, sizeof(double));
    std::memcpy(&ub, &b, sizeof(double));
    return ua == ub;
}

bool f_f_tilde(double a, double b, double tolerance) {
    if (tolerance > 0.0) {
        return std::fabs(a - b) < tolerance;
    }
    else if (std::fabs(tolerance) < EPSILON) {
        return bitwise_equal(a, b);
    }
    else {
        double scaled_tolerance =
            std::fabs(tolerance) * (std::fabs(a) + std::fabs(b));
        return std::fabs(a - b) < scaled_tolerance;
    }
}

