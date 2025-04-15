#include "core/screen.hpp"
#include "ui/render.hpp"  // Include the render header to access the rendering functions
#include "core/editor.hpp"  // Include editor header to access EditorState and Buffer

void clearScreen() {
    // Clear the console using winConsole API
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};

    // Get the console screen buffer info to calculate screen size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Fill the console with spaces (clear screen)
    DWORD dwWritten;
    FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, topLeft, &dwWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, topLeft, &dwWritten);

    // Reset cursor position to the top-left corner
    setCursorPosition(0, 0);
}

void drawLine(int row, const std::string& text) {
    // Set the cursor at the beginning of the specified row
    setCursorPosition(0, row);
    
    // Print the text for that row
    std::cout << text;
}

void refreshScreen(EditorState& editor) {
    // Refresh the screen by clearing and redrawing necessary components
    clearScreen();

    // Get the buffer from editor
    Buffer& buf = editor.getBuffer();

    // Call the render functions with the correct arguments
    renderBuffer(buf, editor);  // Pass the buffer and editor state
    renderStatusBar(editor);    // Pass the editor state
    renderCursor(editor);       // Pass the editor state
}