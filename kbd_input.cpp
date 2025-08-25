//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "kbd_input.h"
#include "vm.h"
using namespace std;

// Pack key_code and modifiers into a 32-bit ekey
uint32_t pack_ekey(uint32_t key_code, uint32_t modifiers) {
    return (modifiers & 0xFF000000) | (key_code & 0x00FFFFFF);
}

// Unpack ekey into key_code and modifiers
void unpack_ekey(uint32_t ekey, uint32_t& key_code, uint32_t& modifiers) {
    modifiers = ekey & 0xFF000000;
    key_code = ekey & 0x00FFFFFF;
}

// Check if key_code is printable ASCII
bool is_printable_char(uint32_t key_code) {
    return key_code >= 32 && key_code <= 126;
}

// ekey_to_char: returns character code or 0
uint32_t ekey_to_char(uint32_t ekey) {
    uint32_t key_code, modifiers;
    unpack_ekey(ekey, key_code, modifiers);
    return is_printable_char(key_code) ? key_code : 0;
}

// ekey_to_fkey: returns full function key code with modifiers, or 0
uint32_t ekey_to_fkey(uint32_t ekey) {
    uint32_t key_code, modifiers;
    unpack_ekey(ekey, key_code, modifiers);
    return is_printable_char(key_code) ? 0 : ekey;
}

// Forth interface functions
void f_key_query() {
    push(f_bool(key_available()));
}

void f_key() {
    int key = get_key();
    push(key);
}

void f_ekey_query() {
    push(f_bool(key_available()));
}

void f_ekey() {
    uint32_t ekey = get_ekey();
    push(ekey);
}

void f_ekey_to_char() {
    uint32_t ekey = pop();
    int c = ekey_to_char(ekey);
    if (c == 0) {
        push(ekey);
        push(F_FALSE);
    }
    else {
        push(c);
        push(F_TRUE);
    }
}

void f_ekey_to_fkey() {
    uint32_t ekey = pop();
    int fkey = ekey_to_fkey(ekey);
    if (fkey == 0) {
        push(ekey);
        push(F_FALSE);
    }
    else {
        push(fkey);
        push(F_TRUE);
    }
}
