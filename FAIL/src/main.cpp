#include <iostream>
#include <string>
#include "core/editor.hpp"
#include "core/input.hpp"
#include "core/screen.hpp"
#include "config/config.hpp"
#include "config/theme.hpp"
#include "filesystem/pathUtils.hpp"
#include "ui/winConsole.hpp"

// Function declarations
void initializeEditor();
void openFile(const std::string& filePath);
void mainLoop();
void shutdownEditor();

// Global editor state
EditorState editorState;
bool isRunning = true;

void initializeEditor() {
    // Console setup
    initConsole();
    enableRawMode();
    hideCursor();

    // Load config
    applyDefaultConfig();
    try {
        loadConfig("config/nite.conf");
        std::string themeName = getConfigValue("theme");
        loadTheme(themeName.empty() ? "default" : themeName);
    } catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\nFalling back to defaults.\n";
        applyDefaultConfig();
        loadTheme("default");
    }

    // Editor setup
    editorState.initEditor();
    clearScreen();
    editorState.setMode(EditorMode::Normal);
    editorState.renderEditor();
}

void openFile(const std::string& filePath) {
    if (fileExists(filePath)) {
        editorState.getBuffer().loadFile(filePath);
    } else {
        editorState.getBuffer().initBuffer();
    }
}

void mainLoop() {
    while (isRunning) {
        InputEvent event = pollInput();

        // Exit condition
        if (!event.isChar && event.specialKey == SpecialKey::Escape &&
            editorState.getMode() == EditorMode::Normal) {
            isRunning = false;
            break;
        }

        // Mode transitions
        if (!event.isChar && event.specialKey == SpecialKey::Escape) {
            editorState.setMode(EditorMode::Normal);
        } else if (event.isChar && event.character == 'i' &&
                   editorState.getMode() == EditorMode::Normal) {
            editorState.setMode(EditorMode::Insert);
        } else if (event.isChar && event.character == ':' &&
                   editorState.getMode() == EditorMode::Normal) {
            editorState.setMode(EditorMode::Command);
        }

        editorState.updateEditor(event);
        editorState.renderEditor();
    }
}

void shutdownEditor() {
    showCursor();
    disableRawMode();
    clearScreen();
    setCursorPosition(0, 0);
    std::cout << "Thank you for using Nite Editor!" << std::endl;
}

int main(int argc, char* argv[]) {
    initializeEditor();

    if (argc > 1) {
        openFile(argv[1]);
    } else {
        editorState.getBuffer().initBuffer();
    }

    mainLoop();
    shutdownEditor();

    return 0;
}
