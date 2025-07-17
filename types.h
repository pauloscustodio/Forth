//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
using namespace std;

// types
typedef uint8_t			uchar;
typedef unsigned int	uint;
typedef int64_t			dint;
typedef uint64_t		udint;

// types sizes
inline static const int CHAR_SZ = static_cast<int>(sizeof(char));
inline static const int CELL_SZ = static_cast<int>(sizeof(int));
inline static const int DCELL_SZ = static_cast<int>(sizeof(dint));

// VM size
inline static const int MEM_SZ = 256 * 1024;
inline static const int BUFFER_SZ = 1024;
inline static const int PAD_SZ = 256;
inline static const int WORDBUF_SZ = BUFFER_SZ;
inline static const int TIB_SZ = BUFFER_SZ;
inline static const int STACK_SZ = 1024 * sizeof(int);
inline static const int MAX_WORD_SZ = 255;

// constants
inline static const char BL = ' ';
inline static const char CR = '\n';
inline static const int SCREEN_WIDTH = 80;

inline static const int F_TRUE = -1;
inline static const int F_FALSE = 0;

// alignment and double cells
inline int align(int x) {
	return (x + CELL_SZ - 1) & ~(CELL_SZ - 1);
}
inline int dcell_lo(dint x) {
	return x & 0xffffffffLL;
}
inline int dcell_hi(dint x) {
	return (x >> 32) & 0xffffffffLL;
}
inline int dcell(int hi, int lo) {
	return (static_cast<udint>(dcell_lo(hi)) << 32) |
		static_cast<udint>(dcell_lo(lo));
}
