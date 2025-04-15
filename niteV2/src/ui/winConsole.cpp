#include "ui/winConsole.hpp"

static HANDLE hConsole = nullptr;
static DWORD originalMode = 0;

void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hConsole, &originalMode);
}

void setCursorPosition(int x, int y) {
    COORD position = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hConsole, position);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void enableRawMode() {
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    mode |= ENABLE_PROCESSED_INPUT;
    SetConsoleMode(hConsole, mode);
}

void disableRawMode() {
    SetConsoleMode(hConsole, originalMode);
}

// Method to get the screen dimensions (width and height)
std::pair<int, int> getScreenDimensions() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return {csbi.srWindow.Right - csbi.srWindow.Left + 1,
                csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
    }
    // In case of error, return (0, 0)
    return {0, 0};
}
