#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Quick helper function that returns the digits of a number; used for aligning things.
int countDigits(int n) {
    if (n == 0) return 1;
    int digits = 0;
    while (n > 0) {
        n /= 10;
        digits++;
    }
    return digits;
}

struct Action {
    enum Type {
        InsertChar,
        DeleteChar,
        InsertLine,
        DeleteLine,
        InsertString,
        DeleteSelection
    };

    Type type;
    int cursorX, cursorY;
    std::string text;  // For InsertChar, DeleteChar, InsertLine, and DeleteLine
    std::string oldText;  // For InsertChar, DeleteChar, InsertLine, and DeleteLine
    int selStartX, selStartY;  // For selection operations
    int selEndX, selEndY;      // For selection operations
};

std::vector<Action> undoStack;
std::vector<Action> redoStack;

class Editor {
public:
    int screenRows, screenCols;
    int cursorX = 0, cursorY = 0;
    int rowOffset = 0, colOffset = 0;
    bool dirty = false;
    std::string filename;
    std::vector<std::string> lines;
    bool skipHorizontalScroll = false;
    const int tabSize = 4;
    
    // Selection variables
    bool hasSelection = false;
    int selectionStartX = 0;
    int selectionStartY = 0;
    int selectionEndX = 0;
    int selectionEndY = 0;

    Editor() {
        getWindowSize(screenRows, screenCols);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD newSize = { static_cast<SHORT>(screenCols), static_cast<SHORT>(screenRows) };
        SetConsoleScreenBufferSize(hOut, newSize);
    }

    void getWindowSize(int &rows, int &cols) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }

    void moveCursor(int row, int col) {
        COORD coord = { static_cast<SHORT>(col), static_cast<SHORT>(row) };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void drawStatusBar(std::ostringstream &sb) {
        std::string status = "[ErrorWriter] " + (filename.empty() ? "[No Name]" : filename);
        if (dirty) status += " (modified)";
        if (hasSelection) status += " (text selected)";
        
        // Add cursor position (X, Y)
        status += " | Row: " + std::to_string(cursorY + 1) + " | Col: " + std::to_string(cursorX + 1);  // +1 to make it 1-based
        
        if ((int)status.size() < screenCols)
            sb << status << std::string(screenCols - status.size(), ' ');
        else
            sb << status.substr(0, screenCols);
    }
    
    // Function to determine if a position is within selection
    bool isPositionSelected(int fileRow, int fileCol) {
        if (!hasSelection) return false;
        
        // Normalize selection coordinates (start should be before end)
        int startX, startY, endX, endY;
        normalizeSelection(startX, startY, endX, endY);
        
        if (fileRow < startY || fileRow > endY) return false;
        if (fileRow == startY && fileCol < startX) return false;
        if (fileRow == endY && fileCol >= endX) return false;
        
        return true;
    }
    
    // Normalize selection coordinates to ensure start is always before end
    void normalizeSelection(int &startX, int &startY, int &endX, int &endY) {
        if (selectionStartY < selectionEndY) {
            startX = selectionStartX;
            startY = selectionStartY;
            endX = selectionEndX;
            endY = selectionEndY;
        } else if (selectionStartY > selectionEndY) {
            startX = selectionEndX;
            startY = selectionEndY;
            endX = selectionStartX;
            endY = selectionStartY;
        } else { // Same line
            startY = selectionStartY;
            endY = selectionEndY;
            if (selectionStartX <= selectionEndX) {
                startX = selectionStartX;
                endX = selectionEndX;
            } else {
                startX = selectionEndX;
                endX = selectionStartX;
            }
        }
    }

    void drawEditor() {
        std::ostringstream buffer;
        buffer.str(""); buffer.clear();
        
        // Normalized selection coordinates if selection exists
        int startX = 0, startY = 0, endX = 0, endY = 0;
        if (hasSelection) {
            normalizeSelection(startX, startY, endX, endY);
        }
        
        // Handle line drawing with selection highlight using ANSI escape codes
        for (int y = 0; y < screenRows - 1; ++y) {
            int fileRow = y + rowOffset;
            std::string lineNumberPart;
        
            if (fileRow < (int)lines.size()) {
                lineNumberPart = std::to_string(fileRow + 1);  // +1 for 1-based line numbering
            } else {
                lineNumberPart = "~";  // Empty tilde line
            }
            // Pad the line number with spaces
            while (lineNumberPart.size() < static_cast<size_t>(std::to_string(std::max(1, (int)lines.size())).length()))
                lineNumberPart = " " + lineNumberPart;
        
            // Separator
            lineNumberPart += " | ";
        
            // Construct line display
            std::string displayLine = lineNumberPart;
        
            if (fileRow < (int)lines.size()) {
                std::string& line = lines[fileRow];
        
                // Cut off by column offset and screen width minus the line number gutter
                int lineEnd = std::min((int)line.size(), colOffset + screenCols - (int)displayLine.size());
                for (int x = colOffset; x < lineEnd; ++x) {
                    displayLine += line[x];
                }
            }
        
            // Pad the rest with spaces
            if ((int)displayLine.size() < screenCols) {
                displayLine += std::string(screenCols - displayLine.size(), ' ');
            }
        
            buffer << displayLine;
        }
        
        // Draw status bar
        drawStatusBar(buffer);
        
        // Get the entire buffer as a string
        std::string output = buffer.str();

        // Handle cursor position
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        
        // Create a buffer of attributes for the text
        std::vector<WORD> attributes(output.size(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        
        int lineNumberWidth = std::to_string(std::max(1, (int)lines.size())).length();
        int gutterWidth = lineNumberWidth + 3;  // 3 for " | "

        // Set selection attributes
        if (hasSelection) {
            for (int y = 0; y < screenRows - 1; ++y) {
                int fileRow = y + rowOffset;
                if (fileRow < (int)lines.size()) {
                    for (int x = 0; x < screenCols && x + colOffset < (int)lines[fileRow].size(); ++x) {
                        int bufferPos = y * screenCols + x;
                        if (bufferPos < (int)attributes.size()) {
                            bool isSelected = ((fileRow > startY && fileRow < endY) ||
                                              (fileRow == startY && fileRow == endY && x + colOffset >= startX && x + colOffset < endX) ||
                                              (fileRow == startY && fileRow != endY && x + colOffset >= startX) ||
                                              (fileRow == endY && fileRow != startY && x + colOffset < endX));
                            
                            if (isSelected) {
                                attributes[bufferPos + gutterWidth] = BACKGROUND_BLUE | BACKGROUND_INTENSITY | 
                                                      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                            }
                        }
                    }
                }
            }
        }
        
        // Write the buffer to console
        DWORD written;
        WriteConsoleOutputCharacterA(hOut, output.c_str(), output.size(), {0, 0}, &written);
        
        // Write the attributes
        for (int y = 0; y < screenRows; ++y) {
            int startIdx = y * screenCols;
            int length = screenCols;
            if (startIdx + length <= (int)attributes.size()) {
                WriteConsoleOutputAttribute(hOut, &attributes[startIdx], length, {0, (SHORT)y}, &written);
            }
        }
        
        // Position the cursor
        moveCursor(cursorY - rowOffset, cursorX - colOffset + gutterWidth);
    }

    void scroll() {
        if (cursorY < rowOffset) rowOffset = cursorY;
        if (cursorY >= rowOffset + screenRows - 1)
            rowOffset = cursorY - screenRows + 2;
        if (!skipHorizontalScroll) {
            if (cursorX < colOffset)
                colOffset = cursorX;
            if (cursorX >= colOffset + screenCols)
                colOffset = cursorX - screenCols + 1;
        } else {
            skipHorizontalScroll = false;
        }
    }

    void scrollToLine(int lineNumber) {
        // Clamp line number to valid range
        lineNumber = std::max(0, std::min((int)lines.size() - 1, lineNumber));
    
        // Scroll so that the line appears ~1/3 from top, or as high as possible
        int targetRowOffset = lineNumber - screenRows / 3;
        rowOffset = std::max(0, targetRowOffset);
    
        // Move cursorY to the line we scrolled to
        cursorY = lineNumber;
    
        // Reset horizontal scroll too, if necessary
        if (!skipHorizontalScroll) {
            if (cursorX < colOffset)
                colOffset = cursorX;
            if (cursorX >= colOffset + screenCols)
                colOffset = cursorX - screenCols + 1;
        } else {
            skipHorizontalScroll = false;
        }
    }    

    void insertChar(char c) {
        // If there's a selection, delete it first
        if (hasSelection) {
            deleteSelection();
        }
        
        if (cursorY >= (int)lines.size()) {
            lines.resize(cursorY + 1);
        }
    
        // Store the current state before inserting
        Action action;
        action.type = Action::InsertChar;
        action.cursorX = cursorX;
        action.cursorY = cursorY;
        action.text = std::string(1, c);  // The character inserted
    
        undoStack.push_back(action);  // Push the action to the undo stack
        redoStack.clear();            // Clear redo stack after new action
    
        lines[cursorY].insert(lines[cursorY].begin() + cursorX, c);
        cursorX++;
        dirty = true;
    }

    void insertTab() {
        for (int i = 0; i < tabSize; ++i)
            insertChar(' ');
    }

    void deleteChar() {
        // If there's a selection, delete it instead
        if (hasSelection) {
            deleteSelection();
            return;
        }
        
        if (cursorY >= (int)lines.size()) return;
        if (cursorX > 0) {
            char deletedChar = lines[cursorY][cursorX - 1];
            
            // Store the current state before deleting
            Action action;
            action.type = Action::DeleteChar;
            action.cursorX = cursorX;
            action.cursorY = cursorY;
            action.oldText = std::string(1, deletedChar);  // The deleted character
            
            undoStack.push_back(action);  // Push the action to the undo stack
            redoStack.clear();            // Clear redo stack after new action
            
            lines[cursorY].erase(lines[cursorY].begin() + cursorX - 1);
            cursorX--;
        } else if (cursorY > 0) {
            int prevLen = (int)lines[cursorY - 1].size();
            std::string deletedLine = lines[cursorY];

            // Store the current state before merging
            Action action;
            action.type = Action::DeleteLine;
            action.cursorX = cursorX;
            action.cursorY = cursorY;
            action.oldText = deletedLine;

            undoStack.push_back(action);  // Push the action to the undo stack
            redoStack.clear();            // Clear redo stack after new action

            lines[cursorY - 1] += lines[cursorY];
            lines.erase(lines.begin() + cursorY);
            cursorY--;
            cursorX = prevLen;
        }
        dirty = true;
    }

    void deleteWord() {
        // If there's a selection, delete it instead
        if (hasSelection) {
            deleteSelection();
            return;
        }
        
        if (cursorY >= (int)lines.size()) return;
        while (cursorX > 0 && lines[cursorY][cursorX - 1] == ' ') {
            deleteChar();
        }
        while (cursorX > 0 && lines[cursorY][cursorX - 1] != ' ') {
            deleteChar();
        }
        // Reset the flag after word deletion
        skipHorizontalScroll = false;
    }
    
    void insertNewLine() {
        // If there's a selection, delete it first
        if (hasSelection) {
            deleteSelection();
        }
        
        if (cursorY >= (int)lines.size()) {
            lines.resize(cursorY + 1); // Ensure the current line exists
        }
        
        // If cursor is at the end of an empty line, we need to handle that gracefully
        if (cursorX > (int)lines[cursorY].size()) {
            cursorX = (int)lines[cursorY].size();  // Prevent out of range access
        }
    
        std::string current = lines[cursorY];
        
        // Check if the cursor position is valid (i.e., don't attempt to substring if the line is shorter than the cursor position)
        std::string newLine = current.substr(cursorX);
        lines[cursorY] = current.substr(0, cursorX);  // Take the part before the cursor
        lines.insert(lines.begin() + cursorY + 1, newLine);  // Insert the rest after the cursor
        
        // Record the action for undo
        Action action;
        action.type = Action::InsertLine;
        action.cursorX = cursorX;
        action.cursorY = cursorY;
        action.text = newLine;
        
        undoStack.push_back(action);
        redoStack.clear();
        
        cursorY++;
        cursorX = 0;  // Move the cursor to the beginning of the new line
        dirty = true;
    }
    
    void startSelection() {
        hasSelection = true;
        selectionStartX = cursorX;
        selectionStartY = cursorY;
        selectionEndX = cursorX;
        selectionEndY = cursorY;
    }
    
    void updateSelection() {
        if (hasSelection) {
            selectionEndX = cursorX;
            selectionEndY = cursorY;
        }
    }
    
    void cancelSelection() {
        hasSelection = false;
    }
    
    // Get selected text
    std::string getSelectedText() {
        if (!hasSelection) return "";
        
        int startX, startY, endX, endY;
        normalizeSelection(startX, startY, endX, endY);
        
        std::string selectedText;
        
        for (int y = startY; y <= endY; y++) {
            if (y >= (int)lines.size()) break;
            
            int lineStartX = (y == startY) ? startX : 0;
            int lineEndX = (y == endY) ? endX : (int)lines[y].size();
            
            if (lineStartX < (int)lines[y].size()) {
                selectedText += lines[y].substr(lineStartX, lineEndX - lineStartX);
            }
            
            if (y < endY) selectedText += "\n";
        }
        
        return selectedText;
    }
    
    // Delete selected text
    void deleteSelection() {
        if (!hasSelection) return;
        
        int startX, startY, endX, endY;
        normalizeSelection(startX, startY, endX, endY);
        
        // Store selection information for undo
        Action action;
        action.type = Action::DeleteSelection;
        action.selStartX = startX;
        action.selStartY = startY;
        action.selEndX = endX;
        action.selEndY = endY;
        action.oldText = getSelectedText();
        
        // Handle single line selection
        if (startY == endY) {
            lines[startY].erase(startX, endX - startX);
            cursorX = startX;
            cursorY = startY;
        } 
        // Handle multi-line selection
        else {
            // Handle first line
            std::string firstPart = lines[startY].substr(0, startX);
            
            // Handle last line
            std::string lastPart = "";
            if (endY < (int)lines.size() && endX <= (int)lines[endY].size()) {
                lastPart = lines[endY].substr(endX);
            }
            
            // Combine first and last parts
            lines[startY] = firstPart + lastPart;
            
            // Remove all lines in between
            lines.erase(lines.begin() + startY + 1, lines.begin() + endY + 1);
            
            // Set cursor position
            cursorX = startX;
            cursorY = startY;
        }
        
        undoStack.push_back(action);
        redoStack.clear();
        
        hasSelection = false;
        dirty = true;
    }
    
    // Copy selected text to clipboard (Windows specific)
    void copySelection() {
        if (!hasSelection) return;
        
        std::string selectedText = getSelectedText();
        
        // Open clipboard and clear it
        if (!OpenClipboard(NULL)) return;
        EmptyClipboard();
        
        // Allocate global memory and copy text to it
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, selectedText.size() + 1);
        if (hMem == NULL) {
            CloseClipboard();
            return;
        }
        
        LPTSTR pMem = (LPTSTR)GlobalLock(hMem);
        memcpy(pMem, selectedText.c_str(), selectedText.size() + 1);
        GlobalUnlock(hMem);
        
        // Set clipboard data and close it
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
    }
    
    // Cut selected text (copy + delete)
    void cutSelection() {
        if (!hasSelection) return;
        
        copySelection();
        deleteSelection();
    }
    
    // Paste from clipboard
    void pasteFromClipboard() {
        // If there's a selection, delete it first
        if (hasSelection) {
            deleteSelection();
        }
        
        // Open clipboard
        if (!OpenClipboard(NULL)) return;
        
        // Get clipboard data
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == NULL) {
            CloseClipboard();
            return;
        }
        
        // Lock memory and get text
        char* pText = static_cast<char*>(GlobalLock(hData));
        if (pText == NULL) {
            CloseClipboard();
            return;
        }
        
        std::string clipboardText = pText;
        GlobalUnlock(hData);
        CloseClipboard();
        
        // Insert the clipboard text
        Action action;
        action.type = Action::InsertString;
        action.cursorX = cursorX;
        action.cursorY = cursorY;
        action.text = clipboardText;
        
        // Process text and handle newlines
        std::istringstream stream(clipboardText);
        std::string line;
        bool firstLine = true;
        
        while (std::getline(stream, line)) {
            if (!firstLine) {
                insertNewLine();
            }
            
            // Insert characters from the line
            for (char c : line) {
                if (c == '\t') {
                    insertTab();
                } else {
                    insertChar(c);
                }
            }
            
            firstLine = false;
        }
        
        undoStack.push_back(action);
        redoStack.clear();
        
        dirty = true;
    }
    
    void moveCursorKey(int key, bool withShift) {
        // If Shift is not pressed, cancel any existing selection
        if (!withShift && hasSelection) {
            cancelSelection();
        }
        
        // If Shift is pressed and no selection exists, start a new selection
        if (withShift && !hasSelection) {
            startSelection();
        }
        
        switch (key) {
            case 75: // Left
                if (cursorX > 0) cursorX--;
                else if (cursorY > 0) {
                    cursorY--;
                    cursorX = (int)lines[cursorY].size();
                }
                break;
            case 77: // Right
                if (cursorY < (int)lines.size()) {
                    if (cursorX < (int)lines[cursorY].size()) cursorX++;
                    else if (cursorY + 1 < (int)lines.size()) {
                        cursorY++;
                        cursorX = 0;
                    }
                }
                break;
            case 72: // Up
                if (cursorY > 0) {
                    cursorY--;
                    cursorX = std::min(cursorX, (int)lines[cursorY].size());
                }
                break;
            case 80: // Down
                if (cursorY + 1 < (int)lines.size()) {
                    cursorY++;
                    cursorX = std::min(cursorX, (int)lines[cursorY].size());
                }
                break;
            case 71: // Home
                cursorX = 0;
                break;
            case 79: // End
                if (cursorY < (int)lines.size())
                    cursorX = (int)lines[cursorY].size();
                break;
            case 73: // Page Up
                cursorY = std::max(0, cursorY - (screenRows - 2));
                if (cursorY < (int)lines.size())
                    cursorX = std::min(cursorX, (int)lines[cursorY].size());
                break;
            case 81: // Page Down
                cursorY = std::min((int)lines.size() - 1, cursorY + (screenRows - 2));
                if (cursorY < (int)lines.size())
                    cursorX = std::min(cursorX, (int)lines[cursorY].size());
                break;
        }
        
        // If Shift is pressed, update the selection end point
        if (withShift) {
            updateSelection();
        }
    }

    void openFile(const std::string &fname) {
        filename = fname;
        std::ifstream file(fname);
        std::string line;
        lines.clear();
        while (std::getline(file, line))
            lines.push_back(line);
        if (lines.empty())
            lines.push_back("");
        dirty = false;
        // Reset selection and cursor
        hasSelection = false;
        cursorX = 0;
        cursorY = 0;
    }

    void saveFile() {
        if (filename.empty()) return;
        std::ofstream file(filename);
        for (auto &line : lines) file << line << '\n';
        dirty = false;
    }

    void undo() {
        if (undoStack.empty()) return;  // Nothing to undo
        
        // Cancel any selection
        cancelSelection();
        
        // Pop the latest action from the undo stack
        Action action = undoStack.back();
        undoStack.pop_back();
    
        // Apply the inverse of the action
        switch (action.type) {
            case Action::InsertChar:
                // Undo InsertChar by deleting the inserted character
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size() && 
                    action.cursorX > 0 && action.cursorX <= (int)lines[action.cursorY].size()) {
                    lines[action.cursorY].erase(action.cursorX - 1, 1);
                    cursorX = action.cursorX - 1;
                    cursorY = action.cursorY;
                }
                break;
            case Action::DeleteChar:
                // Undo DeleteChar by inserting the deleted character back
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size() && 
                    action.cursorX >= 0 && action.cursorX <= (int)lines[action.cursorY].size()) {
                    lines[action.cursorY].insert(action.cursorX - 1, action.oldText);
                    cursorX = action.cursorX;
                    cursorY = action.cursorY;
                }
                break;
            case Action::InsertLine:
                // Undo InsertLine by merging lines
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size() - 1) {
                    lines[action.cursorY] += lines[action.cursorY + 1];
                    lines.erase(lines.begin() + action.cursorY + 1);
                    cursorX = action.cursorX;
                    cursorY = action.cursorY;
                }
                break;
            case Action::DeleteLine:
                // Undo DeleteLine by reinserting the deleted line
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size()) {
                    std::string currentLine = lines[action.cursorY];
                    std::string startPart = currentLine.substr(0, action.cursorX);
                    std::string endPart = currentLine.substr(action.cursorX);
                    
                    lines[action.cursorY] = startPart;
                    lines.insert(lines.begin() + action.cursorY + 1, action.oldText + endPart);
                    
                    cursorX = 0;
                    cursorY = action.cursorY + 1;
                }
                break;
            case Action::InsertString:
                // This is more complex - we'd need to delete all the inserted text
                // For simplicity, we'll just move the cursor back
                cursorX = action.cursorX;
                cursorY = action.cursorY;
                break;
            case Action::DeleteSelection:
                // Restore the deleted selection
                if (action.selStartY >= 0 && action.selStartY < (int)lines.size()) {
                    // For single line selection
                    if (action.selStartY == action.selEndY) {
                        lines[action.selStartY].insert(action.selStartX, action.oldText);
                        
                        // Set cursor position and selection
                        cursorX = action.selEndX;
                        cursorY = action.selEndY;
                        selectionStartX = action.selStartX;
                        selectionStartY = action.selStartY;
                        selectionEndX = action.selEndX;
                        selectionEndY = action.selEndY;
                        hasSelection = true;
                    }
                    // For multi-line selection
                    else {
                        std::string originalLine = lines[action.selStartY];
                        std::string firstPart = originalLine.substr(0, action.selStartX);
                        std::string lastPart = originalLine.substr(action.selStartX);
                        
                        std::istringstream stream(action.oldText);
                        std::string line;
                        std::vector<std::string> newLines;
                        
                        // Split the old text into lines
                        while (std::getline(stream, line)) {
                            newLines.push_back(line);
                        }
                        
                        // Handle special case where oldText doesn't end with newline
                        if (!newLines.empty()) {
                            // Update first line
                            lines[action.selStartY] = firstPart + newLines[0];
                            
                            // Insert middle lines
                            for (int i = 1; i < (int)newLines.size(); i++) {
                                lines.insert(lines.begin() + action.selStartY + i, newLines[i]);
                            }
                            
                            // Add the last part to the last inserted line
                            int lastLineIndex = action.selStartY + newLines.size() - 1;
                            lines[lastLineIndex] += lastPart;
                            
                            // Set cursor position and selection
                            cursorX = action.selEndX;
                            cursorY = action.selEndY;
                            selectionStartX = action.selStartX;
                            selectionStartY = action.selStartY;
                            selectionEndX = cursorX;
                            selectionEndY = cursorY;
                            hasSelection = true;
                        }
                    }
                }
                break;
        }
    
        redoStack.push_back(action);  // Push the undone action to the redo stack
    }    

    void redo() {
        if (redoStack.empty()) return;  // Nothing to redo
        
        // Cancel any selection
        cancelSelection();
    
        // Pop the latest action from the redo stack
        Action action = redoStack.back();
        redoStack.pop_back();
    
        // Apply the action again
        switch (action.type) {
            case Action::InsertChar:
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size() && 
                    action.cursorX >= 0 && action.cursorX <= (int)lines[action.cursorY].size()) {
                    lines[action.cursorY].insert(action.cursorX, action.text);
                    cursorX = action.cursorX + 1;
                    cursorY = action.cursorY;
                }
                break;
            case Action::DeleteChar:
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size() && 
                    action.cursorX > 0 && action.cursorX <= (int)lines[action.cursorY].size()) {
                    lines[action.cursorY].erase(action.cursorX - 1, 1);
                    cursorX = action.cursorX - 1;
                    cursorY = action.cursorY;
                }
                break;
            case Action::InsertLine:
                if (action.cursorY >= 0 && action.cursorY < (int)lines.size()) {
                    std::string currentLine = lines[action.cursorY];
                    std::string firstPart = currentLine.substr(0, action.cursorX);
                    std::string lastPart = currentLine.substr(action.cursorX);
                    
                    lines[action.cursorY] = firstPart;
                    lines.insert(lines.begin() + action.cursorY + 1, action.text + lastPart);
                    
                    cursorX = 0;
                    cursorY = action.cursorY + 1;
                }
                break;
            case Action::DeleteLine:
                if (action.cursorY > 0 && action.cursorY < (int)lines.size()) {
                    lines[action.cursorY - 1] += lines[action.cursorY];
                    lines.erase(lines.begin() + action.cursorY);
                    
                    cursorX = action.cursorX;
                    cursorY = action.cursorY - 1;
                }
                break;
            case Action::InsertString:
                // For simplicity, just move cursor
                cursorX = action.cursorX;
                cursorY = action.cursorY;
                break;
            case Action::DeleteSelection:
                if (action.selStartY >= 0 && action.selStartY < (int)lines.size()) {
                    // Set selection
                    selectionStartX = action.selStartX;
                    selectionStartY = action.selStartY;
                    selectionEndX = action.selEndX;
                    selectionEndY = action.selEndY;
                    hasSelection = true;
                    
                    // Delete selection
                    deleteSelection();
                }
                break;
        }
    
        undoStack.push_back(action);  // Push the redone action to the undo stack
    }

    void processInput() {
        scroll();
        drawEditor();
        
        while (true) {
            int c = _getch();
            
            // Check for shift key state
            bool shiftPressed = GetKeyState(VK_SHIFT) < 0;
            bool ctrlPressed = GetKeyState(VK_CONTROL) < 0;
            
            // Check for Ctrl + Z (undo)
            if (c == 26) {  // ASCII value for Ctrl+Z
                undo();
            }
            // Check for Ctrl + Y (redo)
            else if (c == 25) {  // ASCII value for Ctrl+Y
                redo();
            }
            // Check for Ctrl + X (cut)
            else if (c == 24) {  // ASCII value for Ctrl+X
                cutSelection();
            }
            // Check for Ctrl + C (copy)
            else if (c == 3) {  // ASCII value for Ctrl+C
                copySelection();
            }
            // Check for Ctrl + V (paste)
            else if (c == 22) {  // ASCII value for Ctrl+V
                pasteFromClipboard();
            }
            // Check for Ctrl + A (select all)
            else if (c == 1) {  // ASCII value for Ctrl+A
                // Select all text
                hasSelection = true;
                selectionStartX = 0;
                selectionStartY = 0;
                selectionEndX = lines.empty() ? 0 : lines.back().size();
                selectionEndY = lines.size() - 1;
                cursorX = selectionEndX;
                cursorY = selectionEndY;
            }
            // Handle special key input (like arrow keys, etc.)
            else if (c == 224) {  // Special key (like arrow keys, etc.)
                int c2 = _getch();
                
                if (c2 >= 1 && c2 <= 31) {
                    continue;  // Ignore control characters
                }
                
                moveCursorKey(c2, shiftPressed);
            }
            else if (c == 27) {  // Escape - cancel selection and/or exit
                if (hasSelection) {
                    cancelSelection();
                } else {
                    break;  // Exit if no selection
                }
            } else if (c == '\t') {  // Tab
                insertTab();
            } else if (c == '\r') {  // Enter
                insertNewLine();
            } else if (c == 8) {  // Backspace
                deleteChar();
            } else if (c == 127) { // DEL key
                deleteWord();
            } else if (c == 19) {  // Ctrl+S (Save file)
                saveFile();
            } else if (c == 15) {  // Ctrl+O (Open file)
                std::cout << "Enter filename to open: ";
                std::string fname;
                std::getline(std::cin, fname);
                openFile(fname);
            } else if (c == 46 && ctrlPressed) {  // Ctrl+. (Delete selection)
                if (hasSelection) {
                    deleteSelection();
                }
            } else if (c == 17) {  // Ctrl+Q (Quit)
                break;
            } else if (c == 7) {
                std::cout << "Enter line number to scroll to: ";
                std::string lineStr;
                std::getline(std::cin, lineStr);
                int line = std::stoi(lineStr) - 1;  // Convert to zero-based index
                std::cout << line << std::endl;
                scrollToLine(line);
            }else if (c >= 32 && c <= 126) {  // Printable characters
                // Insert the character
                insertChar((char)c);
            }
            
            // Ensure the scroll state is correct after each key press
            scroll();
            drawEditor();
        }
    }
};

int main(int argc, char* argv[]) {
    // Set console code page to UTF-8
    SetConsoleOutputCP(65001);
    
    // Enable VT100 terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    Editor editor;
    if (argc >= 2)
        editor.openFile(argv[1]);
    else
        editor.lines.push_back("");
    editor.processInput();
    return 0;
}