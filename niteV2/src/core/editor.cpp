#include "core/editor.hpp"
#include "core/screen.hpp"
#include "ui/winConsole.hpp"

EditorState::EditorState()
    : mode(EditorMode::Normal), cursorRow(0), cursorCol(0), topLine(0) {}

void EditorState::initEditor() {
    initConsole();
    enableRawMode();
    hideCursor();
}

void EditorState::updateEditor(InputEvent e) {
    if (e.isChar && mode == EditorMode::Insert) {
        buffer.insertChar(cursorRow, cursorCol, e.character);
        ++cursorCol;
    } else if (!e.isChar) {
        switch (e.specialKey) {
            case SpecialKey::ArrowUp:
                if (cursorRow > 0) {
                    --cursorRow;
                } else if (topLine > 0) {
                    --topLine;
                }
                break;
            case SpecialKey::ArrowDown:
                if (cursorRow < getScreenDimensions().second - 2 &&
                    topLine + cursorRow + 1 < buffer.lines.size()) {
                    ++cursorRow;
                } else if (topLine + getScreenDimensions().second - 2 < buffer.lines.size()) {
                    ++topLine;
                }
                break;
            case SpecialKey::ArrowLeft:
                if (cursorCol > 0) --cursorCol;
                break;
            case SpecialKey::ArrowRight:
                if (cursorCol < buffer.getLine(cursorRow + topLine).length()) ++cursorCol;
                break;
            default:
                break;
        }
    }
}

void EditorState::renderEditor() {
    refreshScreen(*this);
}

Buffer& EditorState::getBuffer() {
    return buffer;
}

void EditorState::setMode(EditorMode m) {
    mode = m;
}

EditorMode EditorState::getMode() const {
    return mode;
}

void EditorState::setCursor(int row, int col) {
    cursorRow = row;
    cursorCol = col;
}

int EditorState::getCursorRow() const {
    return cursorRow;
}

int EditorState::getCursorCol() const {
    return cursorCol;
}