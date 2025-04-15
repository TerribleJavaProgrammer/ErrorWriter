#include "ui/render.hpp"
#include "ui/winConsole.hpp"

void renderBuffer(Buffer& buf, EditorState& editor) {
    auto [screenWidth, screenHeight] = getScreenDimensions();
    int bufferLines = buf.lines.size();
    int cursorRow = editor.getCursorRow();
    int cursorCol = editor.getCursorCol();
    
    int viewHeight = screenHeight - 1; // Leave one line for status bar
    
    // Horizontal scrolling logic
    int leftCol = 0;
    if (cursorCol >= screenWidth) {
        leftCol = cursorCol - screenWidth + 10;  // Show some context
    }

    // Vertical scrolling logic
    int topLine = editor.topLine;
    int linesToRender = std::min(bufferLines - topLine, viewHeight);

    for (int i = 0; i < linesToRender; ++i) {
        std::string line = buf.getLine(topLine + i);
        setCursorPosition(0, i);
        
        // Render the visible part of the line
        if (leftCol < line.length()) {
            std::string visibleLine = line.substr(leftCol, screenWidth);
            std::cout << visibleLine;
        }
        
        // Clear the rest of the line
        std::cout << std::string(screenWidth - std::min((int)line.length() - leftCol, screenWidth), ' ');
    }

    // Clear remaining lines
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
    int row = editor.getCursorRow();
    int col = editor.getCursorCol();
    int screenRow = row;
    setCursorPosition(col, screenRow);
    showCursor();
}
 