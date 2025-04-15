#pragma once

#include "buffer.hpp"
#include "input.hpp"
#include "ui/render.hpp"

// Enum for different modes of the editor (e.g., Normal, Insert, Command)
enum class EditorMode {
    Normal,
    Insert,
    Command
};

// Struct for EditorState (the state of the editor)
class EditorState {
    public:
        int topLine = 0;
        // Constructor
        EditorState();

        // Editor initialization and updates
        void initEditor();
        void updateEditor(InputEvent e);
        void renderEditor();

        // Accessors and mutators
        Buffer& getBuffer();
        void setMode(EditorMode mode);
        EditorMode getMode() const;
        void setCursor(int row, int col);
        int getCursorRow() const;
        int getCursorCol() const;

    private:
        Buffer buffer;              // The text buffer being edited
        EditorMode mode;            // Current mode (e.g., Normal, Insert)
        int cursorRow;              // Row position of the cursor
        int cursorCol;              // Column position of the cursor
};