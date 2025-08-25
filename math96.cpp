//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

/*
 * Portable ANS-style M* and M* / for 32-bit cells (FIXED)
 * - No __int128, no compiler intrinsics
 * - M*    : ( n1 n2 -- d )       32x32 -> signed 64
 * - M* /  : ( d n1 +n2 -- d )    (signed 64 * signed 32) / unsigned 32
 *           with a true 96-bit intermediate and **floored division**
 *
 * Compile test:
 *   cc -std=c99 -O2 mstar32_fixed.c -DTEST_MSTAR32 && ./a.out
 */

#include "forth.h"
#include "math96.h"
#include "vm.h"
#include <cassert>
using namespace std;

/* ------- Forth cell / double ------- */
typedef int32_t  cell;    /* one Forth cell (signed, 32-bit) */
typedef uint32_t ucell;

typedef struct {
    cell lo, hi;
} dcell;   /* Forth double: lo first, then hi */

/* ------- Unsigned helpers: 64-bit (2 words) & 96-bit (3 words) ------- */
typedef struct {
    ucell lo, hi;
} u64;    /* magnitude of a signed double */
typedef struct {
    ucell lo, mid, hi;
} u96;

/* Add with carry (32-bit words) */
static inline ucell addc32(ucell a, ucell b, ucell* carry) {
    uint64_t s = (uint64_t)a + b + *carry;
    *carry = (ucell)(s >> 32);
    return (ucell)s;
}

/* Sub with borrow (32-bit words) -- kept for completeness (not used in fixes) */
static inline ucell subb32(ucell a, ucell b, ucell* borrow) {
    uint64_t d = (uint64_t)a - b - *borrow;
    *borrow = (ucell)((d >> 63) & 1u); /* 1 if underflow */
    return (ucell)d;
}

/* Convert signed dcell to unsigned magnitude u64 (two's complement abs)
   FIXED: propagate carry from low to high correctly. */
static u64 dcell_abs_u64(dcell d) {
    u64 u = { (ucell)d.lo, (ucell)d.hi };
    if (d.hi < 0) {
        /* two's complement negate (64-bit in two 32-bit words) */
        ucell carry = 1;
        u.lo = ~u.lo;
        u.lo = addc32(u.lo, 0u, &carry); /* +1, carry updated */
        u.hi = ~u.hi;
        u.hi = addc32(u.hi, 0u, &carry); /* add carry into hi */
    }
    return u;
}

/* Convert unsigned magnitude u64 to signed dcell, applying a sign flag
   FIXED: two's complement negation done as (~x) + 1 using addc32 and carry. */
static dcell u64_to_signed_dcell(u64 u, int negative) {
    if (negative) {
        ucell carry = 1;
        u.lo = ~u.lo;
        u.lo = addc32(u.lo, 0u, &carry);
        u.hi = ~u.hi;
        u.hi = addc32(u.hi, 0u, &carry);
    }
    dcell r;
    r.lo = (cell)u.lo;
    r.hi = (cell)u.hi;
    return r;
}

/* 64x32 -> 96 unsigned multiply: (A.hi:A.lo) * b */
static u96 u64_mul_u32(u64 A, ucell b) {
    u96 acc;
    uint64_t p0 = (uint64_t)A.lo * b; /* up to 64 bits */
    uint64_t p1 = (uint64_t)A.hi * b; /* up to 64 bits */

    acc.lo = (ucell)(p0 & 0xFFFFFFFFu);
    uint64_t carry = (p0 >> 32);

    uint64_t mid = (p1 & 0xFFFFFFFFu) + carry;
    acc.mid = (ucell)(mid & 0xFFFFFFFFu);
    acc.hi  = (ucell)((p1 >> 32) + (mid >> 32));
    return acc; /* full 96-bit product */
}

/*
 * Fast word-wise long division in base 2^32:
 *   (N.hi:N.mid:N.lo) / d  where d is 32-bit unsigned and N is 96-bit unsigned
 * Returns quotient Q (96-bit) and remainder R (32-bit).
 *
 * This is the classic "divide by single-limb" algorithm:
 *   q2 = N.hi  / d; r  = N.hi  % d;
 *   q1 = (r<<32 | N.mid) / d; r = ...
 *   q0 = (r<<32 | N.lo)  / d; r = ...
 */
static void u96_div_u32(u96 N, ucell d, u96* Q, ucell* R) {
    assert(d != 0);
    uint64_t r, q;

    /* step 1: high word */
    q = ((uint64_t)N.hi) / d;
    r = ((uint64_t)N.hi) % d;
    Q->hi = (ucell)q;

    /* step 2: middle word */
    uint64_t m = (r << 32) | N.mid;
    q = m / d;
    r = m % d;
    Q->mid = (ucell)q;

    /* step 3: low word */
    uint64_t l = (r << 32) | N.lo;
    q = l / d;
    r = l % d;
    Q->lo = (ucell)q;

    *R = (ucell)r;
}

/* --------- Public API: M* (32x32 -> 64 signed) --------- */
static dcell MSTAR(cell n1, cell n2) {
    int64_t p = (int64_t)n1 * (int64_t)
                n2; /* standard C guarantees at least 64-bit here */
    dcell r;
    r.lo = (cell)(p & 0xFFFFFFFFll);
    r.hi = (cell)((uint64_t)p >> 32);
    return r;
}

/*
 * --------- Public API: M* /  (d n1 + n2 -- d)
 * Inputs:
 *   d   : signed 64-bit (two 32-bit cells in Forth order lo,hi)
 *   n1  : signed 32-bit
 *   n2  : positive 32-bit (ANS says +n2)
 * Semantics:
 *   Compute floor( (d * n1) / n2 ) in signed 64-bit, using a 96-bit intermediate.
 * Implementation:
 *   1) Take magnitudes: |d| (u64), |n1| (u32), D = (uint32_t)n2
 *   2) N = |d| * |n1|  (u96)
 *   3) Q = N / D, R = N % D   (word-wise 96/32 division)
 *   4) If result sign negative and R != 0, increment |Q| by 1 (floored adjustment)
 *   5) Apply sign to |Q| and return as dcell
 */
static dcell MSTAR_SLASH(dcell d, cell n1, cell n2) {
    assert(n2 > 0); /* +n2 by spec */

    int neg_d = (d.hi < 0);
    int neg_n1 = (n1 < 0);
    int neg_out = neg_d ^ neg_n1;

    /* Magnitudes */
    u64   A = dcell_abs_u64(d);
    ucell B = (neg_n1 ? (ucell)(-(int64_t)n1) : (ucell)n1);
    ucell D = (ucell)n2;

    /* 1) 64x32 -> 96 product */
    u96 N = u64_mul_u32(A, B);

    /* 2) 96/32 -> quotient Q (96), remainder R (32) */
    u96 Q;
    ucell R;
    u96_div_u32(N, D, &Q, &R);

    /* 3) Take low 64 bits of Q as magnitude result (Q.mid:Q.lo) */
    u64 mag = { Q.lo, Q.mid };

    /* 4) Floored adjustment: if negative result and remainder non-zero, |Q|++ */
    if (neg_out && R != 0) {
        ucell carry = 0;
        mag.lo = addc32(mag.lo, 1u, &carry);
        mag.hi = addc32(mag.hi, 0u, &carry); /* propagate */
    }

    /* 5) Apply sign */
    return u64_to_signed_dcell(mag, neg_out);
}

void f_m_star() {
    int n2 = pop();
    int n1 = pop();
    dcell result = MSTAR(n1, n2);
    dint result1 = mk_dcell(result.hi, result.lo);
    dpush(result1);
}

void f_m_star_slash() {
    int n2 = pop();
    int n1 = pop();
    dint d = dpop();

    if (n2 == 0) {
        error(Error::DivisionByZero);
    }

    dcell d1;
    d1.lo = dcell_lo(d);
    d1.hi = dcell_hi(d);

    dcell result = MSTAR_SLASH(d1, n1, n2);
    dint result1 = mk_dcell(result.hi, result.lo);
    dpush(result1);
}

