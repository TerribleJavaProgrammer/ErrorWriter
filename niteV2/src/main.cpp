#include <iostream>
#include <string>
#include "core/editor.hpp"
#include "core/input.hpp"
#include "core/screen.hpp"
#include "config/config.hpp"
#include "config/theme.hpp"
#include "filesystem/pathUtils.hpp"
#include "ui/winConsole.hpp"

// function declarations
void initializeEditor();
void processInput();
void renderEditor();
void mainLoop();
void shutdownEditor();
void openFile(const std::string& filePath);

// Global variables for the editor
EditorState editorState;
bool isRunning = true;

void initializeEditor() {
    // Initialize the console
    initConsole();
    enableRawMode();
    hideCursor();
    
    // Load configuration
    applyDefaultConfig();
    try {
        loadConfig("config/nite.conf");
        std::string themeName = getConfigValue("theme");
        if (!themeName.empty()) {
            loadTheme(themeName);
        } else {
            loadTheme("default");
        }
    } catch (const std::exception& e) {
        // If there's an error loading config, fall back to defaults
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        applyDefaultConfig();
        loadTheme("default");
    }
    
    // Initialize the editor state
    editorState.initEditor();
    
    // Clear the screen before starting
    clearScreen();
}

void processInput() {
    InputEvent event = pollInput();
    
    // Check for exit condition (Esc key in Normal mode)
    if (!event.isChar && event.specialKey == SpecialKey::Escape && 
        editorState.getMode() == EditorMode::Normal) {
        isRunning = false;
        return;
    }
    
    // Process mode changes
    if (!event.isChar && event.specialKey == SpecialKey::Escape) {
        // Always return to Normal mode when Escape is pressed
        editorState.setMode(EditorMode::Normal);
    } else if (event.isChar && event.character == 'i' && 
               editorState.getMode() == EditorMode::Normal) {
        // Enter Insert mode with 'i' key in Normal mode
        editorState.setMode(EditorMode::Insert);
    } else if (event.isChar && event.character == ':' && 
               editorState.getMode() == EditorMode::Normal) {
        // Enter Command mode with ':' key in Normal mode
        editorState.setMode(EditorMode::Command);
    }
    
    // Update editor based on the input event
    editorState.updateEditor(event);
}

void renderEditor() {
    // Render the editor state to the screen
    editorState.renderEditor();
}

void mainLoop() {
    while (isRunning) {
        // Process any pending input
        processInput();
        
        // Render the editor state
        renderEditor();
        
        // Small delay to prevent CPU overuse
        Sleep(10);
    }
}

void shutdownEditor() {
    // Show cursor before exiting
    showCursor();
    
    // Disable raw mode to restore normal console behavior
    disableRawMode();
    
    // Display exit message
    clearScreen();
    setCursorPosition(0, 0);
    std::cout << "Thank you for using Nite Editor!" << std::endl;
}

void openFile(const std::string& filePath) {
    if (fileExists(filePath)) {
        // Load the file into the buffer
        editorState.getBuffer().loadFile(filePath);
    } else {
        // Create a new empty buffer
        editorState.getBuffer().initBuffer();
    }
}

int main(int argc, char* argv[]) {
    // Initialize the editor
    initializeEditor();
    
    // Check if a file path was provided as an argument
    if (argc > 1) {
        std::string filePath = argv[1];
        openFile(filePath);
    } else {
        // No file provided, start with an empty buffer
        editorState.getBuffer().initBuffer();
    }
    
    // Enter the main loop
    mainLoop();
    
    // Clean up before exiting
    shutdownEditor();
    
    return 0;
}