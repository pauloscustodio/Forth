//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>

// Modifier masks (high byte)
static const uint32_t MOD_SHIFT = 0x01000000;
static const uint32_t MOD_CTRL = 0x02000000;
static const uint32_t MOD_ALT = 0x04000000;

// Function key codes (low 24 bits)
static const uint32_t KEY_UP = 0x00000200;
static const uint32_t KEY_DOWN = 0x00000201;
static const uint32_t KEY_LEFT = 0x00000202;
static const uint32_t KEY_RIGHT = 0x00000203;
static const uint32_t KEY_HOME = 0x00000204;
static const uint32_t KEY_END = 0x00000205;
static const uint32_t KEY_INSERT = 0x00000206;
static const uint32_t KEY_DELETE = 0x00000207;
static const uint32_t KEY_PRIOR = 0x00000208;
static const uint32_t KEY_NEXT = 0x00000209;
static const uint32_t KEY_F1 = 0x00000210;
static const uint32_t KEY_F2 = 0x00000211;
static const uint32_t KEY_F3 = 0x00000212;
static const uint32_t KEY_F4 = 0x00000213;
static const uint32_t KEY_F5 = 0x00000214;
static const uint32_t KEY_F6 = 0x00000215;
static const uint32_t KEY_F7 = 0x00000216;
static const uint32_t KEY_F8 = 0x00000217;
static const uint32_t KEY_F9 = 0x00000218;
static const uint32_t KEY_F10 = 0x00000219;
static const uint32_t KEY_F11 = 0x0000021A;
static const uint32_t KEY_F12 = 0x0000021B;

void init_console_input();

// Pack key_code and modifiers into a 32-bit ekey
uint32_t pack_ekey(uint32_t key_code, uint32_t modifiers);

// Unpack ekey into key_code and modifiers
void unpack_ekey(uint32_t ekey, uint32_t& key_code, uint32_t& modifiers);

// Check if key_code is printable ASCII
bool is_printable_char(uint32_t key_code);

// ekey_to_char: returns character code or 0
uint32_t ekey_to_char(uint32_t ekey);

// ekey_to_fkey: returns full function key code with modifiers, or 0
uint32_t ekey_to_fkey(uint32_t ekey);

// Check if a key is available (non-blocking)
bool key_available();

// Platform-specific key reader
int get_key();

// Platform-specific ekey reader
uint32_t get_ekey();

// Forth interface functions
void f_key_query();
void f_key();
void f_ekey_query();
void f_ekey();
void f_ekey_to_char();
void f_ekey_to_fkey();
