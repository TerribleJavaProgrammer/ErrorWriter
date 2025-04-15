#include "ui/render.hpp"
#include "ui/winConsole.hpp"

// The buffer that holds the current frame
std::string lastRenderedBuffer;

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

    std::string currentBuffer; // This will hold the buffer we need to render

    for (int i = 0; i < linesToRender; ++i) {
        std::string line = buf.getLine(topLine + i);
        
        // Render the visible part of the line
        if (leftCol < line.length()) {
            std::string visibleLine = line.substr(leftCol, screenWidth);
            currentBuffer += visibleLine;
        }
        
        // Clear the rest of the line
        currentBuffer += std::string(screenWidth - std::min((int)line.length() - leftCol, screenWidth), ' ') + "\n";
    }

    // Clear remaining lines if necessary
    for (int i = linesToRender; i < viewHeight; ++i) {
        currentBuffer += std::string(screenWidth, ' ') + "\n";
    }

    // Only update the screen if the buffer content has changed
    if (currentBuffer != lastRenderedBuffer) {
        setCursorPosition(0, 0);  // Set to the top-left corner for screen clearing
        std::cout << currentBuffer;
        lastRenderedBuffer = currentBuffer;  // Update last rendered buffer
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
              << std::string(screenWidth, ' ');  // Clear the rest of the line
}

void renderCursor(EditorState& editor) {
    int row = editor.getCursorRow();
    int col = editor.getCursorCol();
    
    // Only move cursor if position has changed (avoid unnecessary updates)
    setCursorPosition(col, row);
    showCursor();  // Show the cursor after rendering
}
