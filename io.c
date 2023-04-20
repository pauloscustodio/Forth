//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "io.h"

void f_dump(void) {
    uint len = pop();
    uint addr = pop();
    for (uint i = 0; i < len; i += 16) {
        printf("%08x  ", addr + i);
        for (uint j = i; j < i + 16; j++) {
            if (j < len)
                printf("%02x ", cfetch(addr + j));
            else
                printf("   ");
        }
        for (uint j = i; j < i + 16; j++) {
            if (j < len) {
                char c = cfetch(addr + j);
                printf("%c", c >= 32 && c < 127 ? c : '.');
            }
            else
                printf(" ");
        }
        printf("\n");
    }
}

void f_type(void) {
    uint len = pop();
    uint addr = pop();
    if (len > 0)
        printf("%.*s", len, addr_to_cptr(addr));
}
