#pragma once

#include <windows.h>
#include <conio.h>
#include <utility>

void initConsole();

void setCursorPosition(int x, int y);

void hideCursor();

void showCursor();

void enableRawMode();

void disableRawMode();

std::pair<int, int> getScreenDimensions();
