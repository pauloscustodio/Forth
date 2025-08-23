//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void init_console() {
}

int f_key() {
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

int f_key_query() {
    termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);           // Save terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);         // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply new settings
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);   // Get current file flags
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // Set non-blocking

    int ch = getchar();                       // Try to read a character

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore terminal settings
    fcntl(STDIN_FILENO, F_SETFL, oldf);       // Restore file flags

    if (ch != EOF) {
        ungetc(ch, stdin);                    // Put character back into input buffer
        return true;
    }
    return false;
}

#endif
