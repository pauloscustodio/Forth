//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <limits.h>
#include <stdbool.h>
#include <string.h>

#ifdef _MSC_VER
#define strncasecmp strnicmp
#define strcasecmp  stricmp
#endif

// types
typedef unsigned char       byte;
typedef unsigned int        uint;
typedef long long           dint;
typedef unsigned long long  udint;

#define BYTE_SZ         ((int)sizeof(byte))
#define BYTE_MAX        ((int)UCHAR_MAX)
#define CELL_SZ         ((int)sizeof(int))
#define CELL_MAX        ((int)INT_MAX)
#define DCELL_SZ        ((int)sizeof(dint))
#define DCELL_MAX       ((int)LLONG_MAX)

#define ALIGN(x)        (((x) + CELL_SZ - 1) & ~(CELL_SZ - 1))
#define DCELL_LO(x)     ((x) & 0xffffffffLL)
#define DCELL_HI(x)     (((x) >> 32) & 0xffffffffLL)
#define DCELL(hi, lo)   (((udint)DCELL_LO(hi) << 32) | (udint)DCELL_LO(lo))

#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define MIN(a, b)       ((a) < (b) ? (a) : (b))

#define F_TRUE          -1
#define F_FALSE          0

#define F_BOOL(x)       ((x) ? F_TRUE : F_FALSE)

void init(void);
void f_environment_q(void);
