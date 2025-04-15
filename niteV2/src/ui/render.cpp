#include "ui/render.hpp"
#include "ui/winConsole.hpp"

void renderBuffer(Buffer& buf, EditorState& editor) {
    auto [screenWidth, screenHeight] = getScreenDimensions();
    int bufferLines = buf.lines.size();
    int cursorRow = editor.getCursorRow();

    int viewHeight = screenHeight - 1; // Leave one line for status bar

    // Scroll logic
    if (cursorRow < editor.topLine) {
        editor.topLine = cursorRow;
    } else if (cursorRow >= editor.topLine + viewHeight) {
        editor.topLine = cursorRow - viewHeight + 1;
    }

    int linesToRender = std::min(bufferLines - editor.topLine, viewHeight);

    for (int i = 0; i < linesToRender; ++i) {
        std::string line = buf.getLine(editor.topLine + i);
        setCursorPosition(0, i);
        std::cout << line;
    }

    // Clear remaining lines if any
    for (int i = linesToRender; i < viewHeight; ++i) {
        setCursorPosition(0, i);
        std::cout << std::string(screenWidth, ' ');
    }
}

void renderStatusBar(EditorState& editor) {
    auto [screenWidth, screenHeight] = getScreenDimensions();
    int cursorRow = editor.getCursorRow();
    int cursorCol = editor.getCursorCol();
    EditorMode mode = editor.getMode();

    setCursorPosition(0, screenHeight - 1);
    std::cout << "Mode: " << (mode == EditorMode::Normal ? "Normal" :
                              mode == EditorMode::Insert ? "Insert" : "Command")
              << " | Row: " << cursorRow << " | Col: " << cursorCol
              << std::string(screenWidth, ' ');
}

void renderCursor(EditorState& editor) {
    auto [_, screenHeight] = getScreenDimensions();
    int row = editor.getCursorRow();
    int col = editor.getCursorCol();

    int screenRow = row - editor.topLine;
    if (screenRow >= 0 && screenRow < screenHeight - 1) {
        setCursorPosition(col, screenRow);
    }
}
 