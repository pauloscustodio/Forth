//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2021
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "memory.h"
#include "dict.h"
#include "io.h"
#include "errors.h"
#include "stack.h"
#include <string.h>
#include <stdlib.h>

// VM memory
char mem[MEM_SZ];
User* user = (User*)mem;
Input* input = (Input*)(mem + INPUT0);

void init_memory(void) {
    // zero all memory
    memset(mem, 0, sizeof(mem));

    // and blank all buffers
    memset(user->pad, BL, sizeof(user->pad));
    memset(user->hold, BL, sizeof(user->hold));
    memset(user->wordbuf, BL, sizeof(user->wordbuf));
}

static int check_addr(int addr, int size) {
    if (addr + size > MEM_SZ)
        error(ErrorMemoryOverflow);
    if (size != BYTE_SZ && (addr % CELL_SZ) != 0)
        error(ErrorMemoryAlignment);
    return addr;
}

int fetch(int addr) {
    return *(int*)(mem + check_addr(addr, CELL_SZ));
}

void store(int addr, int value) {
    *(int*)(mem + check_addr(addr, CELL_SZ)) = value;
}

dint dfetch(int addr) {
    int hi = fetch(addr);
    int lo = fetch(addr + CELL_SZ);
    return DCELL(hi, lo);
}

void dstore(int addr, dint value) {
    store(addr, DCELL_HI(value));
    store(addr + CELL_SZ, DCELL_LO(value));
}

int cfetch(int addr) {
    return (byte)mem[check_addr(addr, BYTE_SZ)];
}

void cstore(int addr, int value) {
    mem[check_addr(addr, BYTE_SZ)] = value;
}

const char* fstr_to_cstr(const char* text, int len) {
    static int alloced_len = 0;
    static char* alloced_str = NULL;

    if (alloced_len < len + 1) {
        int new_len = alloced_len ? alloced_len : PAD_SZ;
        while (new_len < len + 1)
            new_len *= 2;

        char* new_str = realloc(alloced_str, new_len);
        if (!new_str)
            error(ErrorOutOfMemory);
        else {
            alloced_len = new_len;
            alloced_str = new_str;
        }
    }

    memcpy(alloced_str, text, len);
    alloced_str[len] = '\0';
    return alloced_str;
}

int fstr_to_counted_str(const char* text, int len) {
    if (len > F_COUNTED_STR_MASK)
        error_arg(ErrorStringTooLong, fstr_to_cstr(text, len));
    user->wordbuf[0] = len;
    memcpy(&user->wordbuf[1], text, len);
    return user->wordbuf - mem;
}

static int _counted_str_size(int len, int len_mask) {
    return ALIGN(1 + (len & len_mask));
}

static const char* _counted_str_fetch(int addr, int len_mask, int* len) {
    *len = cfetch(addr) & len_mask;
    return mem + addr + 1;
}

static void _counted_str_store(int addr, const char* text, int len,
                               int len_mask, int len_flags) {
    check_addr(addr + (len & len_mask), BYTE_SZ);      // check for overflow
    cstore(addr, (len & len_mask) | len_flags);
    memcpy(mem + addr + 1, text, len);
}

int counted_str_size(int len) {
    return _counted_str_size(len, F_COUNTED_STR_MASK);
}

const char* counted_str_fetch(int addr, int* len) {
    return _counted_str_fetch(addr, F_COUNTED_STR_MASK, len);
}

void counted_str_store(int addr, const char* text, int len) {
    _counted_str_store(addr, text, len, F_COUNTED_STR_MASK, 0);
}

int dict_name_size(int len) {
    return _counted_str_size(len, F_LENMASK);
}

const char* dict_name_fetch(int addr, int* len) {
    return _counted_str_fetch(addr, F_LENMASK, len);
}

void dict_name_store(int addr, const char* text, int len, int flags) {
    _counted_str_store(addr, text, len, F_LENMASK, flags);
}

int long_str_size(int len) {
    return ALIGN(CELL_SZ + len);
}

const char* long_str_fetch(int addr, int* len) {
    *len = fetch(addr);
    return mem + addr + CELL_SZ;
}

void long_str_store(int addr, const char* text, int len) {
    check_addr(addr + CELL_SZ + len - 1, BYTE_SZ);      // check for overflow
    store(addr, len);
    memcpy(mem + addr + CELL_SZ, text, len);
}

void f_fill(void) {
    int c = pop();
    int n = pop();
    int addr = pop();
    memset(mem + addr, c, n);
}

void f_erase(void) {
    int n = pop();
    int addr = pop();
    memset(mem + addr, 0, n);
}

void f_move(void) {
    int n = pop();
    int dst = pop();
    int src = pop();
    memmove(mem + dst, mem + src, n);
}
