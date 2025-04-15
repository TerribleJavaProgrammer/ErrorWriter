#ifndef WINCONSOLE_HPP
#define WINCONSOLE_HPP

#include <windows.h>
#include <conio.h>

void initConsole();

void setCursorPosition(int x, int y);

void hideCursor();

void showCursor();

void enableRawMode();

void disableRawMode();

#endif // WINCONSOLE_HPP
