//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "kbd_input.h"

#ifndef _WIN32
#include <cstdio>
#include <fcntl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

void init_console_input() {
}

// Check if a key is available (non-blocking)
bool key_available() {
    fd_set read_fds;
    struct timeval timeout = { 0, 0 }; // no wait

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    return select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &timeout) > 0;
}

// Platform-specific key reader
int get_key() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt); // get current terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // set new attributes
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old attributes
    return ch;
}

// Platform-specific ekey reader
uint32_t get_ekey() {
    uint32_t key_code = 0;
    uint32_t modifiers = 0;

    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    char buf[16] = { 0 };
    int len = read(STDIN_FILENO, buf, sizeof(buf));
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

if (len == 1) {
    key_code = buf[0]; // regular key
}
else if (buf[0] == '\033' && buf[1] == '[') {
    int i = 2;
    int param1 = 0, param2 = 0;

    // Parse first numeric parameter
    while (i < len && buf[i] >= '0' && buf[i] <= '9') {
        param1 = param1 * 10 + (buf[i] - '0');
        i++;
    }

    if (buf[i] == ';') {
        i++;
        // Parse second numeric parameter (modifier)
        while (i < len && buf[i] >= '0' && buf[i] <= '9') {
            param2 = param2 * 10 + (buf[i] - '0');
            i++;
        }
    }

    char final = buf[i];
    switch (final) {
    case 'A': key_code = KEY_UP;    break;
    case 'B': key_code = KEY_DOWN;  break;
    case 'D': key_code = KEY_LEFT;  break;
    case 'C': key_code = KEY_RIGHT; break;
    case 'H': key_code = KEY_HOME;  break;
    case 'F': key_code = KEY_END;   break;
    case '~':
        switch (param1) {
//        case 1:  key_code = KEY_HOME;   break;
        case 2:  key_code = KEY_INSERT; break;
        case 3:  key_code = KEY_DELETE; break;
//        case 4:  key_code = KEY_END;    break;
        case 5:  key_code = KEY_PRIOR;  break;
        case 6:  key_code = KEY_NEXT;   break;
        case 11: key_code = KEY_F1;     break;
        case 12: key_code = KEY_F2;     break;
        case 13: key_code = KEY_F3;     break;
        case 14: key_code = KEY_F4;     break;
        case 15: key_code = KEY_F5;     break;
        case 17: key_code = KEY_F6;     break;
        case 18: key_code = KEY_F7;     break;
        case 19: key_code = KEY_F8;     break;
        case 20: key_code = KEY_F9;     break;
        case 21: key_code = KEY_F10;    break;
        case 23: key_code = KEY_F11;    break;
        case 24: key_code = KEY_F12;    break;
        }
        break;
    }

    // Modifier mapping based on xterm conventions
    switch (param2) {
    case 2: modifiers |= MOD_SHIFT; break;
    case 3: modifiers |= MOD_ALT;   break;
    case 4: modifiers |= MOD_SHIFT | MOD_ALT; break;
    case 5: modifiers |= MOD_CTRL;  break;
    case 6: modifiers |= MOD_SHIFT | MOD_CTRL; break;
    case 7: modifiers |= MOD_ALT | MOD_CTRL; break;
    case 8: modifiers |= MOD_SHIFT | MOD_ALT | MOD_CTRL; break;
    }
}

return pack_ekey(key_code, modifiers);
}

#endif
