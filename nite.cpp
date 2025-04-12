/*
**NITE** - Nimble Interactive Text Editor
*/

#include <iostream>     // Includes the input/output stream library for console I/O operations.
#include <vector>       // Includes the vector container library for dynamic arrays.
#include <string>       // Includes the string class library for string manipulation.
#include <algorithm>    // Includes algorithms like sort(), find(), etc.
#include <windows.h>    // Includes Windows-specific functions for system-level access.
#include <conio.h>      // Includes console I/O functions like getch() and kbhit().
#include <fstream>      // Includes file stream classes for file handling.
#include <sstream>      // Includes string stream classes for reading from and writing to strings.
#include <cstdlib>      // Includes functions like std::exit() and random number generation.

// Struct to represent different types of actions that can be performed in an editor-like environment
struct Action {

    // Enumeration for the different action types supported in this program
    enum Type {
        InsertChar,     // Insert a character at the cursor position
        DeleteChar,     // Delete the character at the cursor position
        InsertLine,     // Insert a new line at the cursor position
        DeleteLine,     // Delete the current line
        InsertString,   // Insert a string at the cursor position
        DeleteSelection,// Delete the selected text between two points
        ReplaceAll      // Replace all instances of a specific string in the text
    };

    Type type;        // The type of action (from the Type enum)
    int cursorX, cursorY;  // Coordinates of the cursor in the editor (X, Y)
    std::string text;  // Holds the text for InsertChar, InsertLine, InsertString actions
    std::string oldText; // Holds the old text for actions that involve text deletion or modification
    int selStartX, selStartY;  // Coordinates of the start of the selection (for selection operations)
    int selEndX, selEndY;      // Coordinates of the end of the selection (for selection operations)
};

std::vector<Action> undoStack;  // A stack (vector) to store actions for undo functionality, allowing you to revert the last action performed.
std::vector<Action> redoStack;  // A stack (vector) to store actions for redo functionality, allowing you to reapply an undone action.

class Editor {
    public:
        int screenRows, screenCols;  // Dimensions of the editor's screen (number of rows and columns visible at a time)
        int cursorX = 0, cursorY = 0;  // Current cursor position (X for horizontal, Y for vertical)
        int rowOffset = 0, colOffset = 0;  // Offsets for scrolling the screen (how much of the file is scrolled)
        bool dirty = false;  // Flag indicating if the file has unsaved changes
        std::string filename;  // The name of the file currently being edited
        std::vector<std::string> lines;  // A vector to store each line of the text file as a string
        bool skipHorizontalScroll = false;  // Flag to control horizontal scrolling behavior
        const int tabSize = 4;  // The number of spaces that should be used for a tab character
        
        // Selection variables
        bool hasSelection = false;  // Flag to indicate if there is a text selection
        int selectionStartX = 0;    // X-coordinate of the start of the selection
        int selectionStartY = 0;    // Y-coordinate of the start of the selection
        int selectionEndX = 0;      // X-coordinate of the end of the selection
        int selectionEndY = 0;      // Y-coordinate of the end of the selection
    
        // Search variables
        std::string searchQuery;    // The current search query string
        int lastSearchPos = -1;     // Position of the last search result
        int lastSearchLine = -1;    // Line number of the last search result
        bool searchActive = false;  // Flag to indicate if a search is currently active
    
        // SB (Status Bar) helper variables
        bool waitingForInput = false;  // Flag to indicate if the editor is waiting for user input (e.g., in a command mode)
        std::string statusPrompt = ""; // A string to store the prompt displayed in the status bar
        std::string statusInput = "";  // A string to store the user's input in the status bar
        bool processingInput = false;  // Flag to indicate if the editor is currently processing user input
        enum InputType { NONE, OPEN_FILE, GOTO_LINE };  // Enum for different types of user input (None, opening a file, or going to a specific line)
        InputType currentInputType = NONE;  // The current type of input being processed (initialized to NONE)

        Editor() {
            getWindowSize(screenRows, screenCols);  // Calls the function getWindowSize to initialize screenRows and screenCols with the current window size.
            
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  // Gets the handle for the standard output (console window) to manipulate its properties.
            
            COORD newSize = { static_cast<SHORT>(screenCols), static_cast<SHORT>(screenRows) };  // Creates a COORD structure to define the new size of the console window, using the previously obtained screen dimensions.
            
            SetConsoleScreenBufferSize(hOut, newSize);  // Sets the screen buffer size of the console window to match the specified dimensions (screenCols, screenRows).
        }

        void getWindowSize(int &rows, int &cols) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;  // Declare a variable of type CONSOLE_SCREEN_BUFFER_INFO to store console screen buffer information.
            
            // Calls GetConsoleScreenBufferInfo to retrieve information about the current console screen buffer and stores it in 'csbi'.
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            
            // Calculates the number of columns by subtracting the left edge from the right edge of the console window.
            cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            
            // Calculates the number of rows by subtracting the top edge from the bottom edge of the console window.
            rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }        

        void moveCursor(int row, int col) {
            COORD coord = { static_cast<SHORT>(col), static_cast<SHORT>(row) };  // Creates a COORD structure to store the new cursor position (column and row).
            
            // Calls SetConsoleCursorPosition to move the console's cursor to the specified position (row, col).
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        }        

        void drawStatusBar(std::ostringstream &sb) {
            if (waitingForInput) {  // Checks if the editor is waiting for user input (e.g., during a prompt).
                // Show prompt and current input
                std::string status = statusPrompt + statusInput;  // Combines the status prompt with the current user input.
                
                // If the status string is shorter than the screen width, pad it with spaces. Otherwise, truncate it.
                if ((int)status.size() < screenCols)
                    sb << status << std::string(screenCols - status.size(), ' ');
                else
                    sb << status.substr(0, screenCols);  // Truncate the status to fit within the screen width.
            } else {  // If not waiting for input, draw the normal status bar.
                // Build the status string with general information (e.g., editor name, filename, modifications, selection state).
                std::string status = "[ErrorWriter] " + (filename.empty() ? "[No Name]" : filename);  // Shows editor name or file name if available.
                
                if (dirty) status += " (modified)";  // Indicates if the file has unsaved changes.
                if (hasSelection) status += " (text selected)";  // Indicates if there is a text selection.
        
                // Adds the cursor position (row and column) to the status.
                status += " | Row: " + std::to_string(cursorY + 1) + " | Col: " + std::to_string(cursorX + 1);  // Converts to 1-based indexing.
        
                // If search is active, include search query details in the status.
                if (searchActive) {
                    status += " | Searching: \"" + searchQuery + "\"";  // Shows the current search query.
                }
                
                // If the status string is shorter than the screen width, pad it with spaces. Otherwise, truncate it.
                if ((int)status.size() < screenCols)
                    sb << status << std::string(screenCols - status.size(), ' ');
                else
                    sb << status.substr(0, screenCols);  // Truncate the status to fit within the screen width.
            }
        }        
    
        void startStatusInput(const std::string &prompt, InputType type) {
            waitingForInput = true;           // Set waitingForInput flag to true, indicating that the editor is awaiting user input.
            statusPrompt = prompt;            // Set the status prompt message, which will be displayed in the status bar.
            statusInput = "";                 // Clear any existing input (ensure the input area is empty when starting).
            currentInputType = type;         // Set the input type (e.g., opening a file, going to a specific line).
            processingInput = true;           // Set the flag to true, indicating that the editor is processing user input.
        }
    
        void processStatusInput() {
            switch (currentInputType) {  // Switch on the type of input that the editor is currently processing.
                case OPEN_FILE:  // If the input type is OPEN_FILE, process the file open command.
                    if (!statusInput.empty()) {  // Check if the user has entered any input.
                        openFile(statusInput);  // Call openFile function with the user input (file name).
                    }
                    break;
                case GOTO_LINE:  // If the input type is GOTO_LINE, process the line navigation command.
                    if (!statusInput.empty()) {  // Check if the user has entered any input.
                        try {
                            int line = std::stoi(statusInput) - 1;  // Convert the input string to an integer and adjust for zero-based indexing.
                            scrollToLine(line);  // Call scrollToLine to move the editor's view to the specified line.
                        } catch (const std::exception& e) {
                            // Handle invalid input (e.g., non-numeric input) gracefully.
                            // You could add a message or error handling here to inform the user.
                        }
                    }
                    break;
                default:
                    break;  // If no valid input type, do nothing.
            }

            // Reset status bar state after processing input
            waitingForInput = false;   // Reset waitingForInput flag to false as input has been processed.
            statusPrompt = "";         // Clear the status prompt message.
            statusInput = "";          // Clear the input entered by the user.
            currentInputType = NONE;   // Reset the input type to NONE.
            processingInput = false;   // Indicate that input processing is finished.
        }

        bool isPositionSelected(int fileRow, int fileCol) {
            if (!hasSelection) return false;  // If there is no selection, return false (no position is selected).
            
            // Normalize selection coordinates (start should be before end)
            int startX, startY, endX, endY;
            normalizeSelection(startX, startY, endX, endY);  // Normalize the selection so that startX, startY is the top-left and endX, endY is the bottom-right.

            // Check if the position is within the selection bounds
            if (fileRow < startY || fileRow > endY) return false;  // If the row is outside the selection range, return false.
            if (fileRow == startY && fileCol < startX) return false;  // If the row is the start of the selection, check if the column is within the start of the selection.
            if (fileRow == endY && fileCol >= endX) return false;  // If the row is the end of the selection, check if the column is within the end of the selection.
            
            return true;  // If all checks pass, the position is within the selection.
        }

        void normalizeSelection(int &startX, int &startY, int &endX, int &endY) {
            if (selectionStartY < selectionEndY) {
                // Selection was made top-to-bottom:
                // start = (selectionStartX, selectionStartY)
                // end   = (selectionEndX,   selectionEndY)
                startX = selectionStartX;
                startY = selectionStartY;
                endX   = selectionEndX;
                endY   = selectionEndY;
            } else if (selectionStartY > selectionEndY) {
                // Selection was made bottom-to-top:
                // swap start and end so start is above end
                startX = selectionEndX;
                startY = selectionEndY;
                endX   = selectionStartX;
                endY   = selectionStartY;
            } else {
                // Selection is on the same line (Y coordinates equal)
                startY = selectionStartY;
                endY   = selectionEndY;
                if (selectionStartX <= selectionEndX) {
                    // Left-to-right selection on the same line
                    startX = selectionStartX;
                    endX   = selectionEndX;
                } else {
                    // Right-to-left selection on the same line: swap X coordinates
                    startX = selectionEndX;
                    endX   = selectionStartX;
                }
            }
        }

        void drawEditor() {
            std::ostringstream buffer;
            buffer.str(""); buffer.clear();  // Clear and reset the output buffer to start fresh

            // Normalized selection coordinates if a selection exists
            int startX = 0, startY = 0, endX = 0, endY = 0;
            if (hasSelection) {
                normalizeSelection(startX, startY, endX, endY);  // Normalize the selection to ensure start is before end
            }

            // Handle line drawing with selection highlight using ANSI escape codes
            for (int y = 0; y < screenRows - 1; ++y) {  // Loop through each screen row, leaving space for the status bar
                int fileRow = y + rowOffset;  // Map screen row to file row, offset by rowOffset
                std::string lineNumberPart;

                // Check if the file has a line for the current row
                if (fileRow < (int)lines.size()) {
                    lineNumberPart = std::to_string(fileRow + 1);  // Line number (1-based index)
                } else {
                    lineNumberPart = "~";  // Empty line indicated by a tilde
                }

                // Pad line number with spaces to match width
                while (lineNumberPart.size() < static_cast<size_t>(std::to_string(std::max(1, (int)lines.size())).length()))
                    lineNumberPart = " " + lineNumberPart;

                // Add separator between line number and actual line content
                lineNumberPart += " | ";

                // Construct the line display
                std::string displayLine = lineNumberPart;

                if (fileRow < (int)lines.size()) {
                    std::string& line = lines[fileRow];

                    // Cut off by column offset and screen width minus the line number gutter
                    int lineEnd = std::min((int)line.size(), colOffset + screenCols - (int)displayLine.size());
                    for (int x = colOffset; x < lineEnd; ++x) {
                        displayLine += line[x];  // Add each character within the visible range
                    }
                }

                // Pad the rest with spaces if the line is shorter than the screen width
                if ((int)displayLine.size() < screenCols) {
                    displayLine += std::string(screenCols - displayLine.size(), ' ');  // Pad the end of the line with spaces
                }

                buffer << displayLine;  // Append the final line content to the buffer
            }

            // Draw the status bar at the bottom
            drawStatusBar(buffer);

            // Get the entire buffer as a string
            std::string output = buffer.str();

            // Handle cursor position (moving the cursor to the correct position on the screen)
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

            // Create a buffer of attributes for the text (initially set to normal foreground colors)
            std::vector<WORD> attributes(output.size(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            int lineNumberWidth = std::to_string(std::max(1, (int)lines.size())).length();  // Width of line number gutter
            int gutterWidth = lineNumberWidth + 3;  // 3 for the separator " | "

            // Set selection attributes (highlight selected text with background color)
            if (hasSelection) {
                for (int y = 0; y < screenRows - 1; ++y) {  // Loop through the rows again to apply selection highlights
                    int fileRow = y + rowOffset;
                    if (fileRow < (int)lines.size()) {
                        for (int x = 0; x < screenCols && x + colOffset < (int)lines[fileRow].size(); ++x) {
                            int bufferPos = y * screenCols + x;
                            if (bufferPos < (int)attributes.size()) {
                                // Check if the current position is within the selection range
                                bool isSelected = ((fileRow > startY && fileRow < endY) ||
                                                (fileRow == startY && fileRow == endY && x + colOffset >= startX && x + colOffset < endX) ||
                                                (fileRow == startY && fileRow != endY && x + colOffset >= startX) ||
                                                (fileRow == endY && fileRow != startY && x + colOffset < endX));

                                // Set selection highlight (blue background with normal text)
                                if (isSelected) {
                                    attributes[bufferPos + gutterWidth] = BACKGROUND_BLUE | BACKGROUND_INTENSITY | 
                                                                        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                                }
                            }
                        }
                    }
                }
            }

            // Write the buffer (text) to console
            DWORD written;
            WriteConsoleOutputCharacterA(hOut, output.c_str(), output.size(), {0, 0}, &written);

            // Write the attributes (colors) to the console
            for (int y = 0; y < screenRows; ++y) {
                int startIdx = y * screenCols;
                int length = screenCols;
                if (startIdx + length <= (int)attributes.size()) {
                    WriteConsoleOutputAttribute(hOut, &attributes[startIdx], length, {0, (SHORT)y}, &written);
                }
            }

            // Position the cursor based on its current position (adjusted by rowOffset and colOffset)
            moveCursor(cursorY - rowOffset, cursorX - colOffset + gutterWidth);
        }
    
        void scroll() {
            // Vertical scroll: if cursor is above the visible area, move the window up
            if (cursorY < rowOffset) rowOffset = cursorY;
            
            // If the cursor is below the visible area, move the window down
            if (cursorY >= rowOffset + screenRows - 1)
                rowOffset = cursorY - screenRows + 2;

            // Horizontal scroll (only if skipHorizontalScroll is not true)
            if (!skipHorizontalScroll) {
                // If the cursor is past the left edge of the screen, move the window left
                if (cursorX < colOffset)
                    colOffset = cursorX;

                // If the cursor is past the right edge of the screen, move the window right
                if (cursorX >= colOffset + screenCols)
                    colOffset = cursorX - screenCols + 1;
            } else {
                // If skipping horizontal scroll, reset the flag for next use
                skipHorizontalScroll = false;
            }
        }

        void scrollToLine(int lineNumber) {
            // Clamp the line number to a valid range: between 0 and the last line of the document
            lineNumber = std::max(0, std::min((int)lines.size() - 1, lineNumber));
            
            // Calculate the target row offset such that the line appears ~1/3 from the top of the screen,
            // or as high as possible if the line is near the top
            int targetRowOffset = lineNumber - screenRows / 3;

            // Ensure the row offset does not go below 0 (i.e., we can't scroll above the first line)
            rowOffset = std::max(0, targetRowOffset);

            // Move the cursor to the specified line
            cursorY = lineNumber;

            // Reset horizontal scrolling if necessary (scroll horizontally to keep cursor within bounds)
            if (!skipHorizontalScroll) {
                // If the cursor is to the left of the visible screen area, scroll left
                if (cursorX < colOffset)
                    colOffset = cursorX;
                
                // If the cursor is to the right of the visible screen area, scroll right
                if (cursorX >= colOffset + screenCols)
                    colOffset = cursorX - screenCols + 1;
            } else {
                // Reset the skip flag if horizontal scroll was previously skipped
                skipHorizontalScroll = false;
            }
        }

        void insertChar(char c) {
            // If there's a text selection, delete it first (inserting character clears the selection)
            if (hasSelection) {
                deleteSelection();  // Clear the selected text
            }

            // Ensure the lines vector has enough rows to accommodate the cursor position
            if (cursorY >= (int)lines.size()) {
                lines.resize(cursorY + 1);  // Add empty lines as needed
            }

            // Store the current state before making changes for undo/redo functionality
            Action action;
            action.type = Action::InsertChar;  // Type of action being performed
            action.cursorX = cursorX;         // Current cursor column position
            action.cursorY = cursorY;         // Current cursor row position
            action.text = std::string(1, c);   // The character being inserted
            
            // Push the action to the undo stack (to allow undo of this operation)
            undoStack.push_back(action);
            
            // Clear the redo stack because new action invalidates the redo history
            redoStack.clear();

            // Insert the character at the cursor position
            lines[cursorY].insert(lines[cursorY].begin() + cursorX, c);

            // Move the cursor to the right after insertion
            cursorX++;

            // Mark the document as modified since changes were made
            dirty = true;
        }

        void insertTab() {
            // Insert 'tabSize' number of spaces to simulate a tab character
            for (int i = 0; i < tabSize; ++i) {
                insertChar(' ');  // Insert a single space at the current cursor position
            }
        }

        void deleteChar() {
            // If there is a text selection, delete it first (inserting character clears the selection)
            if (hasSelection) {
                deleteSelection();  // Remove the selected text
                return;  // Exit the function after handling selection
            }

            // Check if the current cursor row is within the bounds of the lines
            if (cursorY >= (int)lines.size()) return;

            // If the cursor is not at the start of the line (cursorX > 0), delete a character before the cursor
            if (cursorX > 0) {
                // Get the character to be deleted (the one just before the cursor)
                char deletedChar = lines[cursorY][cursorX - 1];
                
                // Store the current state before deleting the character for undo functionality
                Action action;
                action.type = Action::DeleteChar;  // Action type: DeleteChar
                action.cursorX = cursorX;         // Current cursor position (X and Y)
                action.cursorY = cursorY;
                action.oldText = std::string(1, deletedChar);  // The deleted character

                // Push the action to the undo stack for potential undo later
                undoStack.push_back(action);
                
                // Clear the redo stack, as a new action has invalidated the previous redo state
                redoStack.clear();

                // Erase the character before the cursor (cursorX - 1)
                lines[cursorY].erase(lines[cursorY].begin() + cursorX - 1);
                
                // Move the cursor left by one character
                cursorX--;
            }
            // If the cursor is at the beginning of a line and there are previous lines, merge the current line with the previous one
            else if (cursorY > 0) {
                int prevLen = (int)lines[cursorY - 1].size();  // Get the length of the previous line
                std::string deletedLine = lines[cursorY];      // Store the content of the line to be deleted

                // Store the current state before merging lines for undo functionality
                Action action;
                action.type = Action::DeleteLine;  // Action type: DeleteLine (merging lines)
                action.cursorX = cursorX;         // Current cursor position (X and Y)
                action.cursorY = cursorY;
                action.oldText = deletedLine;     // The content of the deleted line

                // Push the action to the undo stack for potential undo later
                undoStack.push_back(action);
                
                // Clear the redo stack as the current action invalidates any redo state
                redoStack.clear();

                // Merge the current line with the previous one (concatenate them)
                lines[cursorY - 1] += lines[cursorY];

                // Erase the current line (cursorY) after merging
                lines.erase(lines.begin() + cursorY);

                // Move the cursor to the previous line and adjust the column position
                cursorY--;
                cursorX = prevLen;  // Set cursor to the end of the merged line
            }

            // Mark the document as modified since a change was made
            dirty = true;
        }

        void deleteWord() {
            // If there's a selection, delete it first
            if (hasSelection) {
                deleteSelection();  // Remove the selected text
                return;  // Exit after handling the selection
            }

            // Check if the current row is valid (within bounds of the lines)
            if (cursorY >= (int)lines.size()) return;

            // First, delete any leading spaces before the word
            while (cursorX > 0 && lines[cursorY][cursorX - 1] == ' ') {
                deleteChar();  // Remove spaces one by one
            }

            // Then, delete the characters of the word itself (until a space is encountered)
            while (cursorX > 0 && lines[cursorY][cursorX - 1] != ' ') {
                deleteChar();  // Remove characters one by one until a space is encountered
            }

            // Reset the horizontal scroll flag after word deletion
            skipHorizontalScroll = false;
        }

        void insertNewLine() {
            // If there's a selection, delete it first
            if (hasSelection) {
                deleteSelection();  // Remove selected text first, if any
            }

            // Ensure the current line exists (resize lines if needed)
            if (cursorY >= (int)lines.size()) {
                lines.resize(cursorY + 1);  // Resize lines vector to accommodate the current row
            }

            // If the cursor is beyond the length of the current line, adjust cursor position
            if (cursorX > (int)lines[cursorY].size()) {
                cursorX = (int)lines[cursorY].size();  // Prevent out of range access
            }

            // Get the current line text
            std::string current = lines[cursorY];
            
            // Check if the cursor position is valid (i.e., don't attempt to substring if the line is shorter than the cursor position)
            std::string newLine = current.substr(cursorX);  // Everything after the cursor becomes the new line
            lines[cursorY] = current.substr(0, cursorX);  // The part before the cursor stays in the current line
            lines.insert(lines.begin() + cursorY + 1, newLine);  // Insert the new line after the current line

            // Record the action for undo
            Action action;
            action.type = Action::InsertLine;  // Action type: insert a new line
            action.cursorX = cursorX;  // Record the cursor position
            action.cursorY = cursorY;  // Record the line number
            action.text = newLine;  // Store the new line text
            
            undoStack.push_back(action);  // Push the action to the undo stack
            redoStack.clear();            // Clear redo stack after a new action
            
            cursorY++;  // Move cursor to the next line
            cursorX = 0;  // Set cursor to the beginning of the new line
            
            dirty = true;  // Mark the document as modified
        }
    
        void startSelection() {
            hasSelection = true;  // Mark that selection has started
            selectionStartX = cursorX;  // Record the starting X position of the selection
            selectionStartY = cursorY;  // Record the starting Y position of the selection
            selectionEndX = cursorX;  // Initially, the end position is the same as the start position
            selectionEndY = cursorY;  // Similarly, the end Y position starts at the current cursor position
        }
    
        void updateSelection() {
            if (hasSelection) {
                selectionEndX = cursorX;  // Update the end X position based on the current cursor X position
                selectionEndY = cursorY;  // Update the end Y position based on the current cursor Y position
            }
        }
        
        void cancelSelection() {
            hasSelection = false;  // Mark that no selection exists
        }
    
        std::string getSelectedText() {
            if (!hasSelection) return "";  // If there's no selection, return an empty string
            
            int startX, startY, endX, endY;
            normalizeSelection(startX, startY, endX, endY);  // Normalize the selection coordinates
            
            std::string selectedText;  // A string to hold the selected text
            
            // Loop through all lines within the selection range
            for (int y = startY; y <= endY; y++) {
                if (y >= (int)lines.size()) break;  // Break if the line exceeds available lines
                
                // Determine the start and end X positions for the current line
                int lineStartX = (y == startY) ? startX : 0;  // If it's the start line, use the start position; otherwise, begin at 0
                int lineEndX = (y == endY) ? endX : (int)lines[y].size();  // If it's the end line, use the end position; otherwise, end at the line's length
                
                // If the line is not empty, append the selected portion of the line to the selected text
                if (lineStartX < (int)lines[y].size()) {
                    selectedText += lines[y].substr(lineStartX, lineEndX - lineStartX);  // Append the substring within the selection
                }
                
                // Add a newline character unless it's the last line
                if (y < endY) selectedText += "\n";
            }
            
            return selectedText;  // Return the final string containing the selected text
        }        

        void deleteSelection() {
            if (!hasSelection) return;  // If there is no active selection, exit the function
            
            int startX, startY, endX, endY;
            normalizeSelection(startX, startY, endX, endY);  // Normalize selection coordinates to ensure correct order
        
            // Store selection information for undo
            Action action;
            action.type = Action::DeleteSelection;
            action.selStartX = startX;
            action.selStartY = startY;
            action.selEndX = endX;
            action.selEndY = endY;
            action.oldText = getSelectedText();  // Save the selected text for undo
            
            // Handle single line selection
            if (startY == endY) {
                lines[startY].erase(startX, endX - startX);  // Erase the selected text from the line
                cursorX = startX;  // Move cursor to the beginning of the remaining text (after deletion)
                cursorY = startY;  // Keep the cursor on the same line
            }
            // Handle multi-line selection
            else {
                // Handle the first part of the first line
                std::string firstPart = lines[startY].substr(0, startX);
                
                // Handle the last part of the last line
                std::string lastPart = "";
                if (endY < (int)lines.size() && endX <= (int)lines[endY].size()) {
                    lastPart = lines[endY].substr(endX);  // Part after the selection in the last line
                }
                
                // Combine the first and last parts into one line
                lines[startY] = firstPart + lastPart;
                
                // Remove the lines in between the selected range (i.e., from startY + 1 to endY inclusive)
                lines.erase(lines.begin() + startY + 1, lines.begin() + endY + 1);
                
                // Set cursor position to the start of the remaining text
                cursorX = startX;
                cursorY = startY;
            }
            
            undoStack.push_back(action);  // Push the action to the undo stack to allow undoing this operation
            redoStack.clear();  // Clear the redo stack, since this is a new operation
            
            hasSelection = false;  // Clear the selection state
            dirty = true;  // Mark the document as dirty (modified)
        }    
    
        void copySelection() {
            if (!hasSelection) return;  // If there's no active selection, do nothing
            
            std::string selectedText = getSelectedText();  // Get the selected text from the editor
            
            // Open the clipboard and clear its contents
            if (!OpenClipboard(NULL)) return;
            EmptyClipboard();
            
            // Allocate global memory to store the selected text
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, selectedText.size() + 1);
            if (hMem == NULL) {
                CloseClipboard();  // If memory allocation fails, close the clipboard
                return;
            }
            
            LPTSTR pMem = (LPTSTR)GlobalLock(hMem);  // Lock the memory for copying text into it
            memcpy(pMem, selectedText.c_str(), selectedText.size() + 1);  // Copy the selected text
            GlobalUnlock(hMem);  // Unlock the memory
            
            // Set the clipboard data to the copied text and close the clipboard
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
        }        
    
        void cutSelection() {
            if (!hasSelection) return;  // If there's no active selection, do nothing
            
            copySelection();  // Copy the selected text to the clipboard
            deleteSelection();  // Delete the selected text from the editor
        }
    
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

        void findNext() {
            if (searchQuery.empty()) return;  // Exit if no search query is provided
        
            // Start from the current cursor position
            int startLine = cursorY;
            int startPos = cursorX;
        
            // If we just found something, move one character forward to avoid finding the same instance
            if (lastSearchLine == startLine && lastSearchPos == startPos && startPos < (int)lines[startLine].size()) {
                startPos++;  // Move one character forward to avoid finding the same instance
            }
        
            // First, search from the current position to the end of the document
            for (int i = startLine; i < (int)lines.size(); i++) {
                // For the first line, start from the current cursor position
                int pos = (i == startLine) ? startPos : 0;
        
                // Find the search query in the current line starting from 'pos'
                size_t found = lines[i].find(searchQuery, pos);
        
                if (found != std::string::npos) {
                    // If found, update cursor position and create a selection for the found text
                    cursorY = i;
                    cursorX = found;
        
                    // Create a selection for the found text
                    hasSelection = true;
                    selectionStartX = found;
                    selectionStartY = i;
                    selectionEndX = found + searchQuery.length();
                    selectionEndY = i;
        
                    // Store the position for the next search
                    lastSearchLine = i;
                    lastSearchPos = found;
        
                    return;  // Return after the match is found and selection is made
                }
            }
        
            // If no match is found from the cursor position to the end of the document, wrap around to the start of the file
            for (int i = 0; i <= startLine; i++) {
                // For the last line (the starting line), search only up to the current cursor position
                int endPos = (i == startLine) ? startPos : (int)lines[i].size();
        
                // Find the query in the current line starting from the beginning
                size_t found = lines[i].find(searchQuery, 0);
        
                if (found != std::string::npos && (i < startLine || found < endPos)) {
                    // If found, update cursor position and create a selection for the found text
                    cursorY = i;
                    cursorX = found;
        
                    // Create a selection for the found text
                    hasSelection = true;
                    selectionStartX = found;
                    selectionStartY = i;
                    selectionEndX = found + searchQuery.length();
                    selectionEndY = i;
        
                    // Store the position for the next search
                    lastSearchLine = i;
                    lastSearchPos = found;
        
                    return;  // Return after the match is found and selection is made
                }
            }
        
            // If no match is found after searching the whole file, keep the cursor where it is
        }        

        void search() {
            // Clear the console at the bottom for input
            moveCursor(screenRows - 1, 0);  // Move cursor to the last line, first column
            std::cout << std::string(screenCols, ' ');  // Clear the entire line
            moveCursor(screenRows - 1, 0);  // Move cursor back to the beginning
            std::cout << "Search: ";  // Print prompt
            
            // Get search input from the user
            std::string query;  // The query the user is typing
            char c;  // Variable to store each character typed by the user
            
            // Loop to read characters until Enter is pressed (carriage return - '\r')
            while ((c = _getch()) != '\r') {
                if (c == 27) { // ESC - cancel the search operation
                    searchActive = false;
                    return;
                } else if (c == 8) { // Backspace key to delete the last character
                    if (!query.empty()) {
                        query.pop_back();  // Remove the last character from the query
                        // Update the display
                        moveCursor(screenRows - 1, 0);  // Move cursor back to the beginning
                        std::cout << "Search: " << query << ' ';  // Redraw the search prompt
                    }
                } else if (c >= 32 && c <= 126) { // Only allow printable characters
                    query += c;  // Add the typed character to the query string
                    // Update the display, moving cursor accordingly
                    moveCursor(screenRows - 1, 8 + query.size() - 1);  // Move cursor to the end of typed query
                    std::cout << c;  // Output the typed character
                }
            }
            
            // When Enter is pressed, search is triggered
            if (!query.empty()) {
                searchQuery = query;  // Set the search query
                searchActive = true;  // Mark the search as active
                lastSearchLine = -1;  // Reset the last search line position
                lastSearchPos = -1;  // Reset the last search position
                findNext();  // Perform the actual search
            }
        }        

        void replaceAll() {
            // If we don't have a search query yet, do a search first
            if (searchQuery.empty()) {
                search();  // Prompt the user to input a search query
                if (searchQuery.empty()) return;  // If no query is entered, return
            }
        
            // Get the replacement text from the user
            moveCursor(screenRows - 1, 0);
            std::cout << std::string(screenCols, ' ');  // Clear the line
            moveCursor(screenRows - 1, 0);
            std::cout << "Replace all with: ";  // Display prompt for replacement text
        
            std::string replaceText;  // The replacement text
            char c;
            
            // Loop to gather user input for the replacement text
            while ((c = _getch()) != '\r') {  // Wait for user input until Enter is pressed
                if (c == 27) {  // ESC - cancel the replace operation
                    return;
                } else if (c == 8) {  // Backspace to remove a character
                    if (!replaceText.empty()) {
                        replaceText.pop_back();  // Remove last character
                        moveCursor(screenRows - 1, 0);
                        std::cout << "Replace all with: " << replaceText << ' ';  // Update display
                    }
                } else if (c >= 32 && c <= 126) {  // Printable characters
                    replaceText += c;  // Add the character to the replacement text
                    moveCursor(screenRows - 1, 17 + replaceText.size() - 1);  // Move cursor
                    std::cout << c;  // Display the character
                }
            }
        
            // Create an action to support undo/redo functionality
            Action action;
            action.type = Action::ReplaceAll;
            action.text = replaceText;        // The new text to replace
            action.oldText = searchQuery;     // The old search query being replaced
        
            int replacementCount = 0;  // Track the number of replacements
        
            // Iterate through each line in the document
            for (int i = 0; i < (int)lines.size(); i++) {
                std::string& line = lines[i];  // Get the reference to the current line
                size_t pos = 0;  // Position from which to start searching
        
                // Loop to find all occurrences of the search query in the line
                while ((pos = line.find(searchQuery, pos)) != std::string::npos) {
                    // Replace the found occurrence with the replacement text
                    line.replace(pos, searchQuery.length(), replaceText);
        
                    // Move position forward to continue searching beyond the current replacement
                    pos += replaceText.length();
                    
                    replacementCount++;  // Increment the replacement count
                }
            }
        
            // If any replacements were made, record the action and show feedback
            if (replacementCount > 0) {
                undoStack.push_back(action);  // Add the action to the undo stack
                redoStack.clear();  // Clear the redo stack
                dirty = true;  // Mark the document as dirty (modified)
        
                // Show the number of replacements in the status bar
                moveCursor(screenRows - 1, 0);
                std::cout << std::string(screenCols, ' ');  // Clear the status line
                moveCursor(screenRows - 1, 0);
                std::cout << "Replaced " << replacementCount << " occurrences.";
                _getch();  // Wait for a key press before continuing
            } else {
                // If no replacements were made, display a message
                moveCursor(screenRows - 1, 0);
                std::cout << std::string(screenCols, ' ');  // Clear the status line
                moveCursor(screenRows - 1, 0);
                std::cout << "No occurrences found.";
                _getch();  // Wait for a key press before continuing
            }
        
            // Cancel selection if any
            cancelSelection();
        }
        
        void openFile(const std::string &fname) {
            filename = fname;  // Store the file name for future reference
            std::ifstream file(fname);  // Open the file for reading
            std::string line;
            
            lines.clear();  // Clear any previously loaded lines
        
            // Read the file line by line and add each line to the `lines` vector
            while (std::getline(file, line)) {
                lines.push_back(line);
            }
        
            // If the file is empty, add an empty line to the document
            if (lines.empty()) {
                lines.push_back("");
            }
        
            dirty = false;  // Set the dirty flag to false (since the file has been successfully loaded)
        
            // Reset selection and cursor
            hasSelection = false;  // No selection after opening a file
            cursorX = 0;  // Move cursor to the beginning of the first line
            cursorY = 0;  // Move cursor to the first line
        }        

        void saveFile() {
            if (filename.empty()) return;  // If the filename is empty, do nothing (no file to save)
            
            std::ofstream file(filename);  // Open the file for writing (this will overwrite existing content)
            
            // Check if the file was opened successfully
            if (!file.is_open()) {
                std::cerr << "Error opening file for saving: " << filename << std::endl;
                return;
            }
        
            // Write each line from the `lines` vector to the file
            for (auto &line : lines) {
                file << line << '\n';  // Write line followed by a newline character
            }
            
            dirty = false;  // Mark the document as saved (no unsaved changes)
        }        

        void undo() {
            if (undoStack.empty()) return;  // Nothing to undo
            
            cancelSelection();  // Cancel any current selection
            
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
                    // Undo InsertString by deleting the inserted text
                    cursorX = action.cursorX;
                    cursorY = action.cursorY;
                    break;
                case Action::DeleteSelection:
                    // Restore the deleted selection
                    if (action.selStartY >= 0 && action.selStartY < (int)lines.size()) {
                        // Single line selection
                        if (action.selStartY == action.selEndY) {
                            lines[action.selStartY].insert(action.selStartX, action.oldText);
                            cursorX = action.selEndX;
                            cursorY = action.selEndY;
                            selectionStartX = action.selStartX;
                            selectionStartY = action.selStartY;
                            selectionEndX = action.selEndX;
                            selectionEndY = action.selEndY;
                            hasSelection = true;
                        }
                        // Multi-line selection
                        else {
                            std::string originalLine = lines[action.selStartY];
                            std::string firstPart = originalLine.substr(0, action.selStartX);
                            std::string lastPart = originalLine.substr(action.selStartX);
                            
                            std::istringstream stream(action.oldText);
                            std::string line;
                            std::vector<std::string> newLines;
                            
                            while (std::getline(stream, line)) {
                                newLines.push_back(line);
                            }
                            
                            if (!newLines.empty()) {
                                lines[action.selStartY] = firstPart + newLines[0];
                                for (int i = 1; i < (int)newLines.size(); i++) {
                                    lines.insert(lines.begin() + action.selStartY + i, newLines[i]);
                                }
                                int lastLineIndex = action.selStartY + newLines.size() - 1;
                                lines[lastLineIndex] += lastPart;
                                
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
                case Action::ReplaceAll:
                    // Undo ReplaceAll by restoring the original text
                    for (int i = 0; i < (int)lines.size(); i++) {
                        std::string& line = lines[i];
                        size_t pos = 0;
                        
                        while ((pos = line.find(action.text, pos)) != std::string::npos) {
                            line.replace(pos, action.text.length(), action.oldText);
                            pos += action.oldText.length();
                        }
                    }
                    break;
            }
            
            redoStack.push_back(action);  // Push the undone action to the redo stack
        }        

        void redo() {
            if (redoStack.empty()) return;  // Nothing to redo
            
            cancelSelection();  // Cancel any current selection
            
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
                case Action::ReplaceAll:
                    // For each line in the document
                    for (int i = 0; i < (int)lines.size(); i++) {
                        std::string& line = lines[i];
                        size_t pos = 0;
                        
                        // While we can find the search query
                        while ((pos = line.find(action.oldText, pos)) != std::string::npos) {
                            // Replace with the replacement text
                            line.replace(pos, action.oldText.length(), action.text);
                            
                            // Move position forward
                            pos += action.text.length();
                        }
                    }
                    break;
            }
            
            undoStack.push_back(action);  // Push the redone action to the undo stack
        }        

        void processInput() {
            // Scroll the content and update the display
            scroll();
            drawEditor();
        
            // Start an infinite loop to handle key input
            while (true) {
                // Get the next character from input (key press)
                int c = _getch();
        
                // If we're waiting for input in the status bar (like filename, line number, etc.)
                if (waitingForInput) {
                    // Handle special input cases for the status bar (like filename input)
                    if (c == 27) {  // Escape key - cancel input
                        waitingForInput = false;
                        statusPrompt = "";
                        statusInput = "";
                        currentInputType = NONE;
                        processingInput = false;
                    } else if (c == '\r') {  // Enter key - submit input
                        processStatusInput();
                    } else if (c == 8) {  // Backspace key - remove last character from input
                        if (!statusInput.empty()) {
                            statusInput.pop_back();
                        }
                    } else if (c >= 32 && c <= 126) {  // Printable characters
                        // Append the character to the current status input
                        statusInput += (char)c;
                    }
        
                    // Recalculate the scroll and redraw the editor after status input
                    scroll();
                    drawEditor();
                    continue;  // Continue to the next iteration if waiting for input
                }
        
                // Check if Shift or Ctrl keys are pressed
                bool shiftPressed = GetKeyState(VK_SHIFT) < 0;
                bool ctrlPressed = GetKeyState(VK_CONTROL) < 0;
        
                // Handle Ctrl+Z (undo action)
                if (c == 26) {  // ASCII value for Ctrl+Z
                    undo();
                }
                // Handle Ctrl+Y (redo action)
                else if (c == 25) {  // ASCII value for Ctrl+Y
                    redo();
                }
                // Handle Ctrl+X (cut selection)
                else if (c == 24) {  // ASCII value for Ctrl+X
                    cutSelection();
                }
                // Handle Ctrl+C (copy selection)
                else if (c == 3) {  // ASCII value for Ctrl+C
                    copySelection();
                }
                // Handle Ctrl+V (paste from clipboard)
                else if (c == 22) {  // ASCII value for Ctrl+V
                    pasteFromClipboard();
                }
                // Handle Ctrl+A (select all text)
                else if (c == 1) {  // ASCII value for Ctrl+A
                    // Select all the text in the document
                    hasSelection = true;
                    selectionStartX = 0;
                    selectionStartY = 0;
                    selectionEndX = lines.empty() ? 0 : lines.back().size();
                    selectionEndY = lines.size() - 1;
                    cursorX = selectionEndX;
                    cursorY = selectionEndY;
                }
                // Handle special key input (like arrow keys, function keys, etc.)
                else if (c == 224) {  // Special key (like arrow keys, etc.)
                    int c2 = _getch();  // Get the second part of the special key
        
                    // Ignore control characters in the range [1, 31]
                    if (c2 >= 1 && c2 <= 31) {
                        continue;
                    }
        
                    // Move the cursor according to the key pressed
                    moveCursorKey(c2, shiftPressed);
                }
                // Handle Ctrl+F (find functionality)
                else if (c == 6) {  // ASCII value for Ctrl+F
                    search();
                }
                // Handle Ctrl+H (replace functionality)
                else if (c == 8 && ctrlPressed) {  // Ctrl+H
                    replaceAll();
                }
                // Handle F3 key for "Find Next"
                else if (c == 0 && _getch() == 61) {  // F3 key
                    findNext();
                }
                // Handle Escape key (cancel selection or exit)
                else if (c == 27) {  // Escape key
                    if (hasSelection) {
                        cancelSelection();  // Cancel any active selection
                    }
                }
                // Handle Tab key (insert tab)
                else if (c == '\t') {  // Tab key
                    insertTab();
                }
                // Handle Enter key (insert a new line)
                else if (c == '\r') {  // Enter key
                    insertNewLine();
                }
                // Handle Backspace key (delete the previous character)
                else if (c == 8) {  // Backspace key
                    deleteChar();
                }
                // Handle Delete key (delete a word)
                else if (c == 127) {  // DEL key
                    deleteWord();
                }
                // Handle Ctrl+S (save the file)
                else if (c == 19) {  // Ctrl+S
                    saveFile();
                }
                // Handle Ctrl+O (open a file)
                else if (c == 15) {  // Ctrl+O
                    startStatusInput("Enter filename to open: ", OPEN_FILE);
                }
                // Handle Ctrl+. (Delete the selected text)
                else if (c == 46 && ctrlPressed) {  // Ctrl+. (Delete selection)
                    if (hasSelection) {
                        deleteSelection();
                    }
                }
                // Handle Ctrl+Q (quit the editor)
                else if (c == 17) {  // Ctrl+Q
                    break;  // Exit the loop (quit)
                }
                // Handle Ctrl+G (go to a specific line)
                else if (c == 7) {
                    startStatusInput("Enter line number to scroll to: ", GOTO_LINE);
                }
                // Handle Ctrl+R (resize the window)
                else if (c == 18) { // ctrl + r (resize)
                    getWindowSize(screenRows, screenCols);
                }
                // Handle printable characters (text input)
                else if (c >= 32 && c <= 126) {  // Printable characters (ASCII)
                    insertChar((char)c);  // Insert the character into the document
                }
        
                // Ensure the scroll position and editor content are updated after each key press
                scroll();
                drawEditor();
            }
        }        
};

int main(int argc, char* argv[]) {
    // Set the console's output code page to UTF-8 for proper handling of Unicode characters
    SetConsoleOutputCP(65001);

    // Enable VT100 terminal sequences (this is for supporting advanced text formatting and colors in the console)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  // Get the handle to the console output
    DWORD dwMode = 0;
    // Get the current console mode
    GetConsoleMode(hOut, &dwMode);
    // Enable the processing of virtual terminal sequences (like color codes)
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    // Set the console mode with the updated options
    SetConsoleMode(hOut, dwMode);

    // Create an instance of the `Editor` class to handle file editing and input processing
    Editor editor;

    // If a file path is provided as a command-line argument (i.e., argc >= 2)
    // Open the file specified in argv[1] and load its content into the editor
    if (argc >= 2)
        editor.openFile(argv[1]);
    else
        // If no file is provided, initialize the editor with an empty line
        editor.lines.push_back("");

    // Begin processing the user's input (key presses, commands, etc.)
    editor.processInput();

    // Return 0 to indicate successful execution
    return 0;
}