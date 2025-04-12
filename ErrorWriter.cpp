#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <sstream>

struct Action {
    enum Type {
        InsertChar,
        DeleteChar,
        InsertLine,
        DeleteLine
    };

    Type type;
    int cursorX, cursorY;
    std::string text;  // For InsertChar, DeleteChar, InsertLine, and DeleteLine
    std::string oldText;  // For InsertChar, DeleteChar, InsertLine, and DeleteLine
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
        if ((int)status.size() < screenCols)
            sb << status << std::string(screenCols - status.size(), ' ');
        else
            sb << status.substr(0, screenCols);
    }

    void drawEditor() {
        std::ostringstream buffer;
        buffer.str(""); buffer.clear();
        for (int y = 0; y < screenRows - 1; ++y) {
            int fileRow = y + rowOffset;
            std::string line;
            if (fileRow < (int)lines.size()) {
                line = lines[fileRow].substr(colOffset, screenCols);
            } else {
                line = "~";
            }
            if ((int)line.size() < screenCols)
                line += std::string(screenCols - line.size(), ' ');
            buffer << line;
        }
        drawStatusBar(buffer);

        std::string out = buffer.str();
        DWORD written;
        WriteConsoleOutputCharacterA(
            GetStdHandle(STD_OUTPUT_HANDLE),
            out.c_str(),
            (DWORD)out.size(),
            {0, 0},
            &written
        );
        moveCursor(cursorY - rowOffset, cursorX - colOffset);
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

    void insertChar(char c) {
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
    
        lines[cursorY].insert(lines[cursorY].begin() + cursorX, c);
        cursorX++;
        dirty = true;
    }

    void insertTab() {
        for (int i = 0; i < tabSize; ++i)
            insertChar(' ');
    }

    void deleteChar() {
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

            lines[cursorY - 1] += lines[cursorY];
            lines.erase(lines.begin() + cursorY);
            cursorY--;
            cursorX = prevLen;
        }
        dirty = true;
    }

    void deleteWord() {
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
        
        cursorY++;
        cursorX = 0;  // Move the cursor to the beginning of the new line
        dirty = true;
    }
    
    void moveCursorKey(int key) {
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
        }
    }

    void openFile(const std::string &fname) {
        filename = fname;
        std::ifstream file(fname);
        std::string line;
        while (std::getline(file, line))
            lines.push_back(line);
        if (lines.empty())
            lines.push_back("");
        dirty = false;
    }

    void saveFile() {
        if (filename.empty()) return;
        std::ofstream file(filename);
        for (auto &line : lines) file << line << '\n';
        dirty = false;
    }

    void undo() {
        if (undoStack.empty()) return;  // Nothing to undo
        
        // Pop the latest action from the undo stack
        Action action = undoStack.back();
        undoStack.pop_back();
    
        // Apply the inverse of the action
        switch (action.type) {
            case Action::InsertChar:
                // Undo InsertChar by deleting the inserted character
                if (cursorY >= 0 && cursorY < (int)lines.size() && cursorX > 0) {
                    lines[cursorY].erase(cursorX - 1, 1);
                    cursorX--;
                }
                break;
            case Action::DeleteChar:
                // Undo DeleteChar by inserting the deleted character back
                if (cursorY >= 0 && cursorY < (int)lines.size()) {
                    lines[cursorY].insert(cursorX, action.oldText);
                    cursorX++;
                }
                break;
            case Action::InsertLine:
                // Undo InsertLine by deleting the inserted line
                if (cursorY < (int)lines.size()) {
                    lines.erase(lines.begin() + cursorY);
                }
                break;
            case Action::DeleteLine:
                // Undo DeleteLine by reinserting the deleted line
                if (cursorY < (int)lines.size()) {
                    lines.insert(lines.begin() + cursorY, action.oldText);
                }
                break;
        }
    
        redoStack.push_back(action);  // Push the undone action to the redo stack
    }    

    void redo() {
        if (redoStack.empty()) return;  // Nothing to redo
    
        // Pop the latest action from the redo stack
        Action action = redoStack.back();
        redoStack.pop_back();
    
        // Apply the action again
        switch (action.type) {
            case Action::InsertChar:
                insertChar(action.text[0]);
                break;
            case Action::DeleteChar:
                deleteChar();
                break;
            case Action::InsertLine:
                // Insert a new line at cursor position
                if (cursorY < (int)lines.size()) {
                    lines.insert(lines.begin() + cursorY, action.text);
                }
                break;
            case Action::DeleteLine:
                // Delete the line at cursor position
                if (cursorY < (int)lines.size()) {
                    lines.erase(lines.begin() + cursorY);
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
            
            // Check for Ctrl + Z (undo)
            if (c == 26) {  // ASCII value for Ctrl+Z
                std::cout << "Undoing last action...\n";
                undo();
            }
            // Check for Ctrl + Y (redo)
            else if (c == 25) {  // ASCII value for Ctrl+Y
                std::cout << "Redoing last action...\n";
                redo();
            }
            // Handle special key input (like arrow keys, etc.)
            else if (c == 224) {  // Special key (like arrow keys, etc.)
                int c2 = _getch();
                
                if (c2 >= 1 && c2 <= 31) {
                    continue;  // Ignore control characters
                }
                
                moveCursorKey(c2);
            }
            else if (c == 27) {  // Escape
                break;
            } else if (c == '\t') {  // Tab
                insertTab();
            } else if (c == '\r') {  // Enter
                insertNewLine();
            } else if (c == 8) {  // Backspace
                deleteChar();
            } else if (c == 19) {  // Ctrl+S (Save file)
                saveFile();
            } else if (c == 15) {  // Ctrl+O (Open file)
                std::cout << "Enter filename to open: ";
                std::string fname;
                std::getline(std::cin, fname);
                openFile(fname);
            } else if (c == 'q') {  // Quit
                break;
            } else {
                // Handle normal characters and insert them
                insertChar((char)c);
            }
            
            // Ensure the scroll state is correct after each key press
            scroll();
            drawEditor();
        }
    }    

};

int main(int argc, char* argv[]) {
    Editor editor;
    if (argc >= 2)
        editor.openFile(argv[1]);
    else
        editor.lines.push_back("");
    editor.processInput();
    return 0;
}
