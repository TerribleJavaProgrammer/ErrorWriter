#include "editor.hpp"

EditorState::EditorState() {
    // Write a constructor for the EditorState class
}

void EditorState::initEditor() {
    // Implement a function to initialize an editor given an editor state
}

void EditorState::updateEditor(InputEvent e) {
    // Given an input event e, update an editor
}

void EditorState::renderEditor() {
    // Given an editor, render it to the screen.
}

void EditorState::setMode(EditorMode mode) {
    // Set the mode of the editor
}

EditorMode EditorState::getMode() const {
    // Get the mode of the editor
}

void EditorState::setCursor(int row, int col) {
    // Set the cursor pos
}

int EditorState::getCursorRow() const {
    // Return the cursor row
}

int EditorState::getCursorCol() const {
    // Return the cursor col
}