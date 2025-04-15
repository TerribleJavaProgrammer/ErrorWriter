#pragma once
#include <string>
#include <iostream>
#include "ui/winConsole.hpp"
#include "ui/render.hpp"

void clearScreen();

void drawLine(int row, const std::string& text);

void refreshScreen(EditorState& editor);
