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
#include <unordered_map> // Includes unordered_map for hash table-like data structures.
#include <filesystem>  // Includes filesystem library for file and directory manipulation.

// === Color Lookup ===
std::unordered_map<std::string, WORD> colorMap = {
    {"black", 0},
    {"dark_blue", FOREGROUND_BLUE},
    {"dark_green", FOREGROUND_GREEN},
    {"dark_cyan", FOREGROUND_GREEN | FOREGROUND_BLUE},
    {"dark_red", FOREGROUND_RED},
    {"dark_magenta", FOREGROUND_RED | FOREGROUND_BLUE},
    {"dark_yellow", FOREGROUND_RED | FOREGROUND_GREEN},
    {"gray", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {"light_gray", FOREGROUND_INTENSITY},
    {"blue", FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {"green", FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {"cyan", FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {"red", FOREGROUND_RED | FOREGROUND_INTENSITY},
    {"magenta", FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    {"yellow", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
    {"white", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
    // Background
    {"bg_black", 0},
    {"bg_dark_blue", BACKGROUND_BLUE},
    {"bg_dark_green", BACKGROUND_GREEN},
    {"bg_dark_cyan", BACKGROUND_GREEN | BACKGROUND_BLUE},
    {"bg_dark_red", BACKGROUND_RED},
    {"bg_dark_magenta", BACKGROUND_RED | BACKGROUND_BLUE},
    {"bg_dark_yellow", BACKGROUND_RED | BACKGROUND_GREEN},
    {"bg_gray", BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE},
    {"bg_blue", BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {"bg_green", BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {"bg_cyan", BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {"bg_red", BACKGROUND_RED | BACKGROUND_INTENSITY},
    {"bg_magenta", BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {"bg_yellow", BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY},
    {"bg_white", BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY},
    {"bg_light_gray", BACKGROUND_INTENSITY}
};

// === Editor Color Variables ===
WORD DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
WORD TYPE_COLOR = FOREGROUND_GREEN;
WORD TYPE_MODIFIER_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
WORD CAST_COLOR = FOREGROUND_GREEN | FOREGROUND_BLUE;
WORD CONTROL_FLOW_COLOR = FOREGROUND_RED | FOREGROUND_GREEN;
WORD OPERATOR_COLOR = FOREGROUND_RED | FOREGROUND_BLUE;
WORD MEMORY_MANAGEMENT_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
WORD EXCEPTION_HANDLING_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
WORD OOP_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
WORD TEMPLATE_COLOR = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
WORD NAMESPACE_COLOR = FOREGROUND_BLUE;
WORD COROUTINE_COLOR = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
WORD CONCEPT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
WORD BOOLEAN_LITERAL_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
WORD NULL_COLOR = FOREGROUND_RED;
WORD PREPROCESSOR_COLOR = FOREGROUND_BLUE | FOREGROUND_GREEN;
WORD MISC_COLOR = FOREGROUND_BLUE | FOREGROUND_RED;
WORD HIGHLIGHT_COLOR = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;

// Some other cool values
bool syntaxHighlighting = false;
int tabSize = 4;

// === Parser ===
void loadColorConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open .niteconfig file.\n";
        return;
    }

    std::unordered_map<std::string, WORD*> colorTargets = {
        {"default", &DEFAULT_COLOR},
        {"type", &TYPE_COLOR},
        {"type_modifier", &TYPE_MODIFIER_COLOR},
        {"cast", &CAST_COLOR},
        {"control_flow", &CONTROL_FLOW_COLOR},
        {"operator", &OPERATOR_COLOR},
        {"memory_management", &MEMORY_MANAGEMENT_COLOR},
        {"exception_handling", &EXCEPTION_HANDLING_COLOR},
        {"oop", &OOP_COLOR},
        {"template", &TEMPLATE_COLOR},
        {"namespace", &NAMESPACE_COLOR},
        {"coroutine", &COROUTINE_COLOR},
        {"concept", &CONCEPT_COLOR},
        {"boolean_literal", &BOOLEAN_LITERAL_COLOR},
        {"null", &NULL_COLOR},
        {"preprocessor", &PREPROCESSOR_COLOR},
        {"misc", &MISC_COLOR},
        {"highlight", &HIGHLIGHT_COLOR}
    };

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments and whitespace
        size_t comment = line.find('#');
        if (comment != std::string::npos)
            line = line.substr(0, comment);
        std::istringstream iss(line);
        std::string key, eq, value;
        if (!(iss >> key >> eq >> value) || eq != "=")
            continue;

        // Normalize the key and value
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        // Process color mappings
        if (colorMap.count(value) && colorTargets.count(key)) {
            *colorTargets[key] = colorMap[value];
        }

        // Process 'syntaxhighlighting' (boolean)
        else if (key == "syntaxhighlighting") {
            if (value == "true") {
                syntaxHighlighting = true;
            } else if (value == "false") {
                syntaxHighlighting = false;
            } else {
                std::cerr << "Invalid value for syntaxhighlighting in config.\n";
            }
        }

        // Process 'tabSize' (integer)
        else if (key == "tabsize") {
            try {
                tabSize = std::stoi(value);  // Convert to integer
                if (tabSize < 1 || tabSize > 8) {
                    std::cerr << "Invalid tabSize value. Must be between 1 and 8.\n";
                    tabSize = 4;  // Default value
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid tabSize value: " << value << "\n";
                tabSize = 4;  // Default value
            } catch (const std::out_of_range& e) {
                std::cerr << "tabSize value out of range: " << value << "\n";
                tabSize = 4;  // Default value
            }
        }
    }
}

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
std::vector<std::string> fileStack; // A stack (vector) to store file names for file navigation, allowing you to go back to previously opened files.

namespace fs = std::filesystem;

class FileNavigator {
    private:
        fs::path currentDirectory;
        std::vector<fs::path> entries;
        int selectedIndex = 0;
        int scrollOffset = 0;
        int screenRows;
        int screenCols;
        
        // Colors for file browser (matching your existing color scheme)
        const WORD DIRECTORY_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        const WORD FILE_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        const WORD SELECTED_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
        
        // Refresh the list of entries in the current directory
        void refreshEntries() {
            entries.clear();
            
            // Add parent directory entry (..) if not at root
            if (currentDirectory.has_parent_path() && 
                currentDirectory != currentDirectory.root_path()) {
                entries.push_back(currentDirectory.parent_path());
            }
            
            // Add all files and directories in the current directory
            for (const auto& entry : fs::directory_iterator(currentDirectory)) {
                entries.push_back(entry.path());
            }
            
            // Sort entries: directories first, then files, both alphabetically
            std::sort(entries.begin(), entries.end(), [](const fs::path& a, const fs::path& b) {
                bool aIsDir = fs::is_directory(a);
                bool bIsDir = fs::is_directory(b);
                
                if (aIsDir && !bIsDir) return true;
                if (!aIsDir && bIsDir) return false;
                
                return a.filename().string() < b.filename().string();
            });
            
            // Reset selection if out of bounds
            if (selectedIndex >= static_cast<int>(entries.size())) {
                selectedIndex = std::max(0, static_cast<int>(entries.size()) - 1);
            }
            
            // Adjust scroll offset if necessary
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + screenRows - 1) {
                scrollOffset = selectedIndex - (screenRows - 1) + 1;
            }
        }
        
    public:
        FileNavigator(const std::string& initialPath, int rows, int cols) 
            : screenRows(rows), screenCols(cols) {
            // Initialize with the given path or current directory if path is invalid
            try {
                currentDirectory = fs::absolute(initialPath);
                if (!fs::exists(currentDirectory) || !fs::is_directory(currentDirectory)) {
                    currentDirectory = fs::current_path();
                }
            } catch (...) {
                currentDirectory = fs::current_path();
            }
            
            refreshEntries();
        }
        
        // Draw the file browser interface
        void drawFileBrowser() {
            std::ostringstream buffer;
            buffer.str(""); 
            buffer.clear();
            
            // Title bar
            std::string title = " File Browser: " + currentDirectory.string() + " ";
            if (title.size() > static_cast<size_t>(screenCols)) {
                // Truncate with ellipsis if too long
                title = " File Browser: ..." + currentDirectory.string().substr(
                    currentDirectory.string().size() - (screenCols - 20)) + " ";
            }
            std::string titleBar = title + std::string(screenCols - title.size(), ' ');
            buffer << titleBar;
            
            // File/directory entries
            std::vector<std::string> displayLines;
            std::vector<WORD> attributes(screenCols * screenRows, FILE_COLOR);
            
            // Set title bar attributes (white on blue background)
            for (int i = 0; i < screenCols; i++) {
                attributes[i] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
            }
            
            // Add each visible entry
            for (int i = 0; i < screenRows - 2; i++) {
                int entryIndex = i + scrollOffset;
                
                if (entryIndex < static_cast<int>(entries.size())) {
                    const fs::path& entry = entries[entryIndex];
                    std::string filename;
                    
                    // Format parent directory specially
                    if (entry == currentDirectory.parent_path()) {
                        filename = "..";
                    } else {
                        filename = entry.filename().string();
                    }
                    
                    // Indicate directories with a trailing slash
                    if (fs::is_directory(entry)) {
                        filename += "/";
                    }
                    
                    // Truncate if too long for display
                    if (filename.size() > static_cast<size_t>(screenCols - 4)) {
                        filename = filename.substr(0, screenCols - 7) + "...";
                    }
                    
                    // Format the display line with padding for selection cursor
                    std::string displayLine = (entryIndex == selectedIndex ? "> " : "  ") + 
                                            filename + 
                                            std::string(screenCols - filename.size() - 2, ' ');
                    
                    buffer << displayLine;
                    
                    // Set attributes for this line
                    int lineStart = (i + 1) * screenCols;
                    WORD baseColor = fs::is_directory(entry) ? DIRECTORY_COLOR : FILE_COLOR;
                    
                    for (int j = 0; j < screenCols; j++) {
                        attributes[lineStart + j] = (entryIndex == selectedIndex) ? SELECTED_COLOR : baseColor;
                    }
                } else {
                    // Empty line
                    buffer << std::string(screenCols, ' ');
                }
            }
            
            // Status bar / help line
            std::string helpLine = " [↑/↓] Navigate  [Enter] Open/Enter  [Esc] Cancel ";
            std::string statusBar = helpLine + std::string(screenCols - helpLine.size(), ' ');
            buffer << statusBar;
            
            // Set status bar attributes (white on blue)
            int statusStart = (screenRows - 1) * screenCols;
            for (int i = 0; i < screenCols; i++) {
                attributes[statusStart + i] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
            }
            
            // Get the entire buffer as a string
            std::string output = buffer.str();
            
            // Write the buffer (text) to console
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD written;
            WriteConsoleOutputCharacterA(hOut, output.c_str(), output.size(), {0, 0}, &written);
            
            // Write the attributes to the console
            for (int y = 0; y < screenRows; y++) {
                int startIdx = y * screenCols;
                WriteConsoleOutputAttribute(hOut, &attributes[startIdx], screenCols, {0, (SHORT)y}, &written);
            }
        }
        
        // Handle keyboard input and navigation
        bool handleInput(int key) {
            switch (key) {
                case VK_UP:    // Up arrow
                    if (selectedIndex > 0) {
                        selectedIndex--;
                        // Adjust scroll if necessary
                        if (selectedIndex < scrollOffset) {
                            scrollOffset = selectedIndex;
                        }
                    }
                    return true;
                    
                case VK_DOWN:  // Down arrow
                    if (selectedIndex < static_cast<int>(entries.size()) - 1) {
                        selectedIndex++;
                        // Adjust scroll if necessary
                        if (selectedIndex >= scrollOffset + screenRows - 2) {
                            scrollOffset = selectedIndex - (screenRows - 2) + 1;
                        }
                    }
                    return true;
                    
                case VK_RETURN:  // Enter key
                    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size())) {
                        const fs::path& selected = entries[selectedIndex];
                        
                        if (fs::is_directory(selected)) {
                            // Enter directory
                            currentDirectory = selected;
                            selectedIndex = 0;
                            scrollOffset = 0;
                            refreshEntries();
                            return true;
                        } else {
                            // Return the selected file path to be opened
                            return false;  // Exit file browser mode
                        }
                    }
                    return true;
                    
                case VK_ESCAPE:  // Escape key
                    return false;  // Cancel and exit file browser mode
                    
                default:
                    return true;  // Ignore other keys
            }
        }
        
        // Get the currently selected file path (if a file is selected)
        fs::path getSelectedFile() const {
            if (selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size())) {
                const fs::path& selected = entries[selectedIndex];
                if (!fs::is_directory(selected)) {
                    return selected;
                }
            }
            return fs::path();  // Empty path if no file selected
        }

        fs::path getCurrentDirectory() const {
            return currentDirectory;
        }
};

class Nite {
    public:
        std::unordered_map<std::string, std::string> cxxKeywords = {
            // **Built-in Types**
            {"bool", "Type"}, {"char", "Type"}, {"char8_t", "Type"}, {"char16_t", "Type"},
            {"char32_t", "Type"}, {"double", "Type"}, {"float", "Type"}, {"int", "Type"},
            {"long", "Type"}, {"short", "Type"}, {"signed", "Type"}, {"unsigned", "Type"},
            {"void", "Type"}, {"wchar_t", "Type"},
        
            // **Type Modifiers / Qualifiers**
            {"const", "Type Modifier"}, {"constexpr", "Type Modifier"}, {"consteval", "Type Modifier"},
            {"constinit", "Type Modifier"}, {"inline", "Type Modifier"}, {"mutable", "Type Modifier"},
            {"volatile", "Type Modifier"}, {"static", "Type Modifier"}, {"register", "Type Modifier"},
            {"thread_local", "Type Modifier"},
        
            // **Type Casts**
            {"const_cast", "Cast"}, {"dynamic_cast", "Cast"}, {"reinterpret_cast", "Cast"},
            {"static_cast", "Cast"},
        
            // **Control Flow**
            {"break", "Control Flow"}, {"case", "Control Flow"}, {"continue", "Control Flow"},
            {"default", "Control Flow"}, {"do", "Control Flow"}, {"else", "Control Flow"},
            {"for", "Control Flow"}, {"goto", "Control Flow"}, {"if", "Control Flow"},
            {"return", "Control Flow"}, {"switch", "Control Flow"}, {"while", "Control Flow"},
        
            // **Logical / Bitwise Operators (Alternative Tokens and Keywords)**
            {"and", "Operator"}, {"and_eq", "Operator"}, {"bitand", "Operator"}, {"bitor", "Operator"},
            {"compl", "Operator"}, {"not", "Operator"}, {"not_eq", "Operator"}, {"or", "Operator"},
            {"or_eq", "Operator"}, {"xor", "Operator"}, {"xor_eq", "Operator"},
        
            // **Symbolic Operators**
            {"+", "Operator"}, {"-", "Operator"}, {"*", "Operator"}, {"/", "Operator"}, {"%", "Operator"},
            {"++", "Operator"}, {"--", "Operator"}, // Arithmetic
            {"==", "Operator"}, {"!=", "Operator"}, {"<", "Operator"}, {">", "Operator"},
            {"<=", "Operator"}, {">=", "Operator"}, // Comparison
            {"!", "Operator"}, {"&&", "Operator"}, {"||", "Operator"}, // Logical
            {"&", "Operator"}, {"|", "Operator"}, {"^", "Operator"}, {"~", "Operator"}, // Bitwise
            {"<<", "Operator"}, {">>", "Operator"}, // Bit shift
            {"=", "Operator"}, {"+=", "Operator"}, {"-=", "Operator"}, {"*=", "Operator"}, {"/=", "Operator"},
            {"%=", "Operator"}, {"&=", "Operator"}, {"|=", "Operator"}, {"^=", "Operator"},
            {"<<=", "Operator"}, {">>=", "Operator"}, // Assignment
            {"?", "Operator"}, {":", "Operator"}, // Ternary
            {".", "Operator"}, {"->", "Operator"}, {"->*", "Operator"}, {".*", "Operator"}, // Member access
            {",", "Operator"}, // Comma
        
            // **Memory Management**
            {"new", "Memory Management"}, {"delete", "Memory Management"},
            {"sizeof", "Memory Management"}, {"alignas", "Memory Management"},
            {"alignof", "Memory Management"},
        
            // **Exception Handling**
            {"try", "Exception Handling"}, {"catch", "Exception Handling"}, {"throw", "Exception Handling"},
        
            // **Object-Oriented Keywords**
            {"struct", "Object-Oriented"}, {"enum", "Object-Oriented"}, {"class", "Object-Oriented"},
            {"friend", "Object-Oriented"}, {"private", "Object-Oriented"}, {"protected", "Object-Oriented"},
            {"public", "Object-Oriented"}, {"this", "Object-Oriented"}, {"virtual", "Object-Oriented"},
        
            // **Templates & Generics**
            {"template", "Template"}, {"typename", "Template"}, {"using", "Template"},
        
            // **Namespace & Modules**
            {"namespace", "Namespace"}, {"export", "Namespace"},
        
            // **Cast/Type-Introspection**
            {"decltype", "Cast/Introspection"}, {"typeid", "Cast/Introspection"},
        
            // **Operator Overloading**
            {"operator", "Operator Overloading"},
        
            // **Boolean Literals**
            {"true", "Boolean Literal"}, {"false", "Boolean Literal"},
        
            // **Null / Undefined**
            {"nullptr", "Null/Undefined"},
        
            // **Preprocessor**
            {"define", "Preprocessor"}, {"include", "Preprocessor"}, {"undef", "Preprocessor"},
            {"ifdef", "Preprocessor"}, {"ifndef", "Preprocessor"}, {"if", "Preprocessor"},
            {"else", "Preprocessor"}, {"elif", "Preprocessor"}, {"endif", "Preprocessor"},
            {"pragma", "Preprocessor"},
        
            // **Coroutines (C++20)**
            {"co_await", "Coroutines"}, {"co_return", "Coroutines"}, {"co_yield", "Coroutines"},
        
            // **Concepts (C++20)**
            {"concept", "Concepts"}, {"requires", "Concepts"},
        
            // **Miscellaneous**
            {"asm", "Miscellaneous"}, {"explicit", "Miscellaneous"}, {"extern", "Miscellaneous"},
            {"noexcept", "Miscellaneous"}, {"static_assert", "Miscellaneous"}
        };
        int screenRows, screenCols;  // Dimensions of the editor's screen (number of rows and columns visible at a time)
        int cursorX = 0, cursorY = 0;  // Current cursor position (X for horizontal, Y for vertical)
        int rowOffset = 0, colOffset = 0;  // Offsets for scrolling the screen (how much of the file is scrolled)
        bool dirty = false;  // Flag indicating if the file has unsaved changes
        std::string filename;  // The name of the file currently being edited
        std::vector<std::string> lines;  // A vector to store each line of the text file as a string
        bool skipHorizontalScroll = false;  // Flag to control horizontal scrolling behavior
        
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

        // File browser mode
        bool inFileBrowserMode = false;
        std::unique_ptr<FileNavigator> fileNavigator;  // Pointer to a FileNavigator object for file browsing functionality

        // File state variables
        std::string currentFile = "";  // Current file path
        bool isModified = false;       // Track if file has unsaved changes

        Nite() {
            loadColorConfig(getNiteConfigPath());  // Load color configuration from the .niteconfig file located in the executable directory.

            getWindowSize(screenRows, screenCols);  // Calls the function getWindowSize to initialize screenRows and screenCols with the current window size.
            
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  // Gets the handle for the standard output (console window) to manipulate its properties.
            
            COORD newSize = { static_cast<SHORT>(screenCols), static_cast<SHORT>(screenRows) };  // Creates a COORD structure to define the new size of the console window, using the previously obtained screen dimensions.
            
            SetConsoleScreenBufferSize(hOut, newSize);  // Sets the screen buffer size of the console window to match the specified dimensions (screenCols, screenRows).
        }

        void enterFileBrowserMode(const std::string& initialPath = "") {
            inFileBrowserMode = true;
            fileNavigator = std::make_unique<FileNavigator>(
                initialPath.empty() ? fs::current_path().string() : initialPath,
                screenRows, screenCols
            );
        }
        
        void exitFileBrowserMode() {
            inFileBrowserMode = false;
            fileNavigator.reset();
        }

        std::string getExecutableDir() {
            char path[MAX_PATH];
            GetModuleFileNameA(NULL, path, MAX_PATH);
        
            std::string fullPath(path);
            size_t lastSlash = fullPath.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                return fullPath.substr(0, lastSlash + 1); // include the slash
            }
            return ""; // fallback
        }
        
        std::string getNiteConfigPath() {
            return getExecutableDir() + ".niteconfig";
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
                std::string status = "[Nite] " + (filename.empty() ? "[No Name]" : filename);  // Shows editor name or file name if available.
                
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

            // Tokenize each line and apply keyword colors
            for (int y = 0; y < screenRows - 1; ++y) {
                int fileRow = y + rowOffset;
                if (fileRow >= (int)lines.size()) continue;

                std::string& line = lines[fileRow];
                int lineEnd = std::min((int)line.size(), colOffset + screenCols - gutterWidth);

                int x = colOffset;
                bool inString = false;  // Track if inside " or ' string

                while (x < lineEnd) {
                    int bufferPos = y * screenCols + (x - colOffset);

                    if (!syntaxHighlighting) {
                        attributes[bufferPos + gutterWidth] = DEFAULT_COLOR;  // Default color for normal text
                        ++x;
                        continue;
                    }

                    // Handle string literals (single or double quotes)
                    if (line[x] == '"' || line[x] == '\'') {
                        inString = !inString;  // Toggle string mode
                        ++x;
                        continue;
                    }

                    // If inside a string, color it blue
                    if (inString) {
                        int start = x;
                        while (x < lineEnd && line[x] != '"' && line[x] != '\'') {
                            ++x;
                        }
                        for (int i = start; i < x; ++i) {
                            int bufferPosStr = y * screenCols + (i - colOffset);
                            attributes[bufferPosStr + gutterWidth] = FOREGROUND_BLUE;
                        }
                        continue;
                    }

                    // Handle comments
                    if (line[x] == '/' && x + 1 < lineEnd && line[x + 1] == '/') {
                        while (x < lineEnd) {
                            int commentPos = y * screenCols + (x - colOffset);
                            attributes[commentPos + gutterWidth] = FOREGROUND_GREEN;  // Dark green
                            ++x;
                        }
                        continue;
                    }

                    // Handle angle-bracketed content
                    if (line[x] == '<') {
                        int start = x;
                        int endPos = line.find('>', x + 1);
                        if (static_cast<std::size_t>(endPos) != std::string::npos
                            && static_cast<std::size_t>(endPos) < static_cast<std::size_t>(lineEnd)) {
                            for (int i = start; i <= endPos; ++i) {
                                int bufferPosBracket = y * screenCols + (i - colOffset);
                                attributes[bufferPosBracket + gutterWidth] = FOREGROUND_RED | FOREGROUND_INTENSITY;
                            }
                            x = endPos + 1;
                            continue;
                        }
                    }

                    // Handle std::
                    if (line.substr(x, 5) == "std::") {
                        int start = x;
                        x += 5;
                        for (int i = start; i < x; ++i) {
                            int bufferPosStd = y * screenCols + (i - colOffset);
                            attributes[bufferPosStd + gutterWidth] = FOREGROUND_RED;
                        }
                        continue;
                    }

                    // Handle operators
                    static const std::vector<std::string> operators = {
                        "==", "!=", "<=", ">=", "->", "::",
                        "+", "-", "*", "/", "%", "=", "<", ">", "&", "|", "^", "~", "!", "++", "--",
                        "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<", ">>", "<<=", ">>="
                    };

                    bool matchedOp = false;
                    for (const auto& op : operators) {
                        if (line.compare(x, op.size(), op) == 0) {
                            for (int i = 0; i < (int)op.size(); ++i) {
                                int pos = y * screenCols + (x + i - colOffset);
                                if (pos + gutterWidth < (int)attributes.size()) {
                                    attributes[pos + gutterWidth] = OPERATOR_COLOR;
                                }
                            }
                            x += op.size();
                            matchedOp = true;
                            break;
                        }
                    }
                    if (matchedOp) continue;

                    // Handle identifiers and keywords
                    if (std::isalpha(line[x]) || line[x] == '_') {
                        int start = x;
                        while (x < lineEnd && (std::isalnum(line[x]) || line[x] == '_')) {
                            ++x;
                        }
                        std::string token = line.substr(start, x - start);

                        // Handle std::
                        if (token == "std" && x < lineEnd && line[x] == ':') {
                            int bufferPosStdOnly = y * screenCols + (start - colOffset);
                            attributes[bufferPosStdOnly + gutterWidth] = FOREGROUND_RED;
                            continue;
                        }

                        auto it = cxxKeywords.find(token);
                        if (it != cxxKeywords.end()) {
                            WORD tokenColor;
                            const std::string &cat = it->second;
                            if      (cat == "Type")                tokenColor = TYPE_COLOR;
                            else if (cat == "Type Modifier")       tokenColor = TYPE_MODIFIER_COLOR;
                            else if (cat == "Cast")                tokenColor = CAST_COLOR;
                            else if (cat == "Cast/Introspection")  tokenColor = CAST_COLOR;
                            else if (cat == "Control Flow")        tokenColor = CONTROL_FLOW_COLOR;
                            else if (cat == "Operator")            tokenColor = OPERATOR_COLOR;
                            else if (cat == "Operator Overloading")tokenColor = OPERATOR_COLOR;
                            else if (cat == "Memory Management")   tokenColor = MEMORY_MANAGEMENT_COLOR;
                            else if (cat == "Exception Handling")  tokenColor = EXCEPTION_HANDLING_COLOR;
                            else if (cat == "Object-Oriented")     tokenColor = OOP_COLOR;
                            else if (cat == "Template")            tokenColor = TEMPLATE_COLOR;
                            else if (cat == "Namespace")           tokenColor = NAMESPACE_COLOR;
                            else if (cat == "Coroutines")          tokenColor = COROUTINE_COLOR;
                            else if (cat == "Concepts")            tokenColor = CONCEPT_COLOR;
                            else if (cat == "Boolean Literal")     tokenColor = BOOLEAN_LITERAL_COLOR;
                            else if (cat == "Null/Undefined")      tokenColor = NULL_COLOR;
                            else if (cat == "Preprocessor")        tokenColor = PREPROCESSOR_COLOR;
                            else if (cat == "Miscellaneous")       tokenColor = MISC_COLOR;
                            else                                   tokenColor = DEFAULT_COLOR;                            

                            for (int i = start; i < x; ++i) {
                                int bufferPosToken = y * screenCols + (i - colOffset);
                                attributes[bufferPosToken + gutterWidth] = tokenColor;
                            }
                        }
                    } else {
                        ++x;
                    }
                }
            }
        
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
                                    attributes[bufferPos + gutterWidth] = HIGHLIGHT_COLOR | 
                                                                            DEFAULT_COLOR;
                                }
                            }
                        }
                    }
                }
            }
        
            // Write the buffer (text) to console
            DWORD written;
            WriteConsoleOutputCharacterA(hOut, output.c_str(), output.size(), {0, 0}, &written);

            // Write the attributes to the console
            for (int y = 0; y < screenRows; ++y) {
                int startIdx = y * screenCols;
                WriteConsoleOutputAttribute(hOut, &attributes[startIdx], screenCols, {0, (SHORT)y}, &written);
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
        
                if (static_cast<std::size_t>(found) != std::string::npos
                    && (static_cast<std::size_t>(i) < static_cast<std::size_t>(startLine)
                    || static_cast<std::size_t>(found) < static_cast<std::size_t>(endPos))) {
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
            // 1) Determine the actual path we want to open
            std::string targetPath;
            if (fname == "config") {
                targetPath = getNiteConfigPath();
            } else if (fname == "prev") {
                // We’ll handle popping below
                if (fileStack.size() < 2) {
                    std::cerr << "No previous file to open.\n";
                    return;
                }
                // Pop off the current file (whatever it is), then open the new top
                fileStack.pop_back();
                targetPath = fileStack.back();
            } else {
                targetPath = fname;
            }
        
            // 2) If this is a “normal” open (not prev), push it onto the stack
            if (fname != "prev") {
                // Avoid duplicate pushes if you open the same path twice in a row
                if (fileStack.empty() || fileStack.back() != targetPath) {
                    fileStack.push_back(targetPath);
                }
            }
        
            // 3) Update your “current filename” state
            filename = targetPath;
        
            // 4) Open the file once, using the resolved targetPath
            std::ifstream file(targetPath);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << targetPath << "\n";
                return;
            }
        
            // 5) Read into your lines buffer
            lines.clear();
            std::string line;
            while (std::getline(file, line)) {
                lines.push_back(line);
            }
            if (lines.empty()) lines.push_back("");
        
            // 6) Reset editor state
            dirty = false;
            hasSelection = false;
            cursorX = cursorY = 0;
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

        void render() {
            if (inFileBrowserMode) {
                fileNavigator->drawFileBrowser();
            } else {
                drawEditor();
            }
        } 

        void processInput() {
            // Scroll the content and update the display
            scroll();
            render();  // Use render() instead of drawEditor() to handle both modes
        
            // Start an infinite loop to handle key input
            while (true) {
                // Get the next character from input (key press)
                int c = _getch();
        
                // If we're in file browser mode, handle file navigation
                if (inFileBrowserMode) {
                    bool shouldStayInBrowser = true;
        
                    if (c == 224) {  // Special key (like arrow keys)
                        int c2 = _getch();  // Get the second part of the special key
                        
                        if (c2 == 72) {  // Up arrow
                            fileNavigator->handleInput(VK_UP);
                        } else if (c2 == 80) {  // Down arrow
                            fileNavigator->handleInput(VK_DOWN);
                        }
                    } else if (c == '\r') {  // Enter key
                        shouldStayInBrowser = fileNavigator->handleInput(VK_RETURN);
                        
                        if (!shouldStayInBrowser) {
                            // User selected a file, open it
                            fs::path selectedFile = fileNavigator->getSelectedFile();
                            inFileBrowserMode = false;
                            
                            if (!selectedFile.empty()) {
                                // Open the selected file
                                openFileFromPath(selectedFile.string());
                            }
                        }
                    } else if (c == 27) {  // Escape key
                        inFileBrowserMode = false;  // Exit file browser mode
                    }
                    
                    render();  // Update the display
                    if (!inFileBrowserMode) {
                        continue;  // Continue to the next iteration if we just exited file browser
                    }
                    
                    continue;  // Skip the rest of the loop when in file browser mode
                }
        
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
                    render();
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
                // Handle Ctrl+N key for "Find Next"
                else if (c == 14) {  // ASCII value for Ctrl+N
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
                // Handle Ctrl+O (open a file using file browser)
                else if (c == 15) {  // Ctrl+O
                    // Use file browser mode instead of text input for opening files
                    inFileBrowserMode = true;
                    fileNavigator = std::make_unique<FileNavigator>(
                        fs::current_path().string(),
                        screenRows, screenCols
                    );
                }
                // Handle Ctrl+. (Delete the selected text)
                else if (c == 46 && ctrlPressed) {  // Ctrl+. (Delete selection)
                    if (hasSelection) {
                        deleteSelection();
                    }
                }
                // Handle Ctrl+Q (quit the editor)
                else if (c == 17) {  // Ctrl+Q
                    system("cls");
                    break;  // Exit the loop (quit)
                }
                // Handle Ctrl+G (go to a specific line)
                else if (c == 7) {
                    startStatusInput("Enter line number to scroll to: ", GOTO_LINE);
                }
                // Handle Ctrl+R (resize the window and reload colors)
                else if (c == 18) { // ctrl + r (resize and reload)
                    getWindowSize(screenRows, screenCols);
                    loadColorConfig(getNiteConfigPath());
                    
                    // Update file browser dimensions if active
                    if (inFileBrowserMode && fileNavigator) {
                        fileNavigator = std::make_unique<FileNavigator>(
                            fileNavigator->getCurrentDirectory().string(),
                            screenRows, screenCols
                        );
                    }
                }
                // Handle syntax higlighting toggle
                else if (c == 20) { // ctrl + t (toggle syntax highlighting)
                    syntaxHighlighting = !syntaxHighlighting;
                }
                // Handle printable characters (text input)
                else if (c >= 32 && c <= 126) {  // Printable characters (ASCII)
                    insertChar((char)c);  // Insert the character into the document
                }
        
                // Ensure the scroll position and editor content are updated after each key press
                scroll();
                render();
            }
        }
        
        void openFileFromPath(const std::string& path) {
            // Clear the current document
            lines.clear();
            cursorX = 0;
            cursorY = 0;
            rowOffset = 0;
            colOffset = 0;
            hasSelection = false;
            
            // Open the file
            std::ifstream file(path);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    // Replace tabs with spaces if needed
                    std::string processedLine;
                    for (char c : line) {
                        if (c == '\t') {
                            processedLine += std::string(tabSize, ' ');
                        } else {
                            processedLine += c;
                        }
                    }
                    lines.push_back(processedLine);
                }
                file.close();
                
                // If file was empty, add one empty line
                if (lines.empty()) {
                    lines.push_back("");
                }
                
                // Update the current filename
                currentFile = path;
                isModified = false;
            } else {
                // Handle file open error
                lines.push_back("");
                startStatusInput("Error: Could not open file. Press any key to continue...", NONE);
            }
        }

        void toggleFileNavigator() {
            // Use file browser mode instead of text input for opening files
            inFileBrowserMode = true;
            fileNavigator = std::make_unique<FileNavigator>(
                fs::current_path().string(),
                screenRows, screenCols
            );
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
    Nite editor;

    // If a file path is provided as a command-line argument (i.e., argc >= 2)
    // Open the file specified in argv[1] and load its content into the editor
    if (argc >= 2)
        if (std::string(argv[1]) == "explorer") {
            editor.toggleFileNavigator();
        } else {
            editor.openFile(argv[1]);
        }
    else
        // If no file is provided, initialize the editor with an empty line
        editor.lines.push_back("");

    // Begin processing the user's input (key presses, commands, etc.)
    editor.processInput();

    // Return 0 to indicate successful execution
    return 0;
}
