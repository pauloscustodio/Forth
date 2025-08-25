//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "kbd_input.h"

#ifdef _WIN32
#include <conio.h>
#include <cstdio>
#include <windows.h>

static HANDLE hStdin = nullptr;

void init_console_input() {
    if (!hStdin) {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdin == INVALID_HANDLE_VALUE) {
            fputs("Failed to get console input handle", stderr);
            exit(EXIT_FAILURE);
        }
    }
}

// Check if a key is available (non-blocking)
bool key_available() {
    return _kbhit();
}

// Platform-specific key reader
int get_key() {
    while (true) {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            /*int special =*/ _getch();
        }
        else {
            return ch;
        }
    }
}

// Platform-specific ekey reader
static uint32_t map_virtual_key(WORD vk) {
    switch (vk) {
    case VK_F1:     return KEY_F1;
    case VK_F2:     return KEY_F2;
    case VK_F3:     return KEY_F3;
    case VK_F4:     return KEY_F4;
    case VK_F5:     return KEY_F5;
    case VK_F6:     return KEY_F6;
    case VK_F7:     return KEY_F7;
    case VK_F8:     return KEY_F8;
    case VK_F9:     return KEY_F9;
    case VK_F10:    return KEY_F10;
    case VK_F11:    return KEY_F11;
    case VK_F12:    return KEY_F12;
    case VK_LEFT:   return KEY_LEFT;
    case VK_RIGHT:  return KEY_RIGHT;
    case VK_UP:     return KEY_UP;
    case VK_DOWN:   return KEY_DOWN;
    case VK_HOME:   return KEY_HOME;
    case VK_END:    return KEY_END;
    case VK_PRIOR:  return KEY_PRIOR;
    case VK_NEXT:   return KEY_NEXT;
    case VK_INSERT: return KEY_INSERT;
    case VK_DELETE: return KEY_DELETE;
    default:        return vk; // fallback to raw VK code
    }
}

uint32_t get_ekey() {
    init_console_input();

    DWORD originalMode;
    GetConsoleMode(hStdin, &originalMode);

    // Switch to raw mode
    SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    INPUT_RECORD input;
    DWORD eventsRead;

    while (true) {
        ReadConsoleInput(hStdin, &input, 1, &eventsRead);

        if (input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) {
            KEY_EVENT_RECORD& key = input.Event.KeyEvent;

            uint32_t key_code = map_virtual_key(key.wVirtualKeyCode);
            uint32_t modifiers = 0;

            if (key.dwControlKeyState & SHIFT_PRESSED)   modifiers |= MOD_SHIFT;
            if (key.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) modifiers |= MOD_CTRL;
            if (key.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))   modifiers |= MOD_ALT;

            // Restore cooked mode
            SetConsoleMode(hStdin, originalMode);

            return pack_ekey(key_code, modifiers);
        }
    }
}

#endif
