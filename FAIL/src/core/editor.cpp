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
        std::string currentLine, beforeCursor, afterCursor;

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
            case SpecialKey::ArrowRight:
                if (cursorCol < buffer.getLine(cursorRow + topLine).length()) {
                    ++cursorCol;
                } else if (cursorRow + topLine < buffer.lines.size() - 1) {
                    // Move to beginning of next line
                    ++cursorRow;
                    cursorCol = 0;
                }
                break;
            case SpecialKey::ArrowLeft:
                if (cursorCol > 0) {
                    --cursorCol;
                } else if (cursorRow + topLine > 0) {
                    // Move to end of previous line
                    --cursorRow;
                    cursorCol = buffer.getLine(cursorRow + topLine).length();
                }
                break;
            case SpecialKey::Enter:
                currentLine = buffer.getLine(cursorRow + topLine);
                beforeCursor = currentLine.substr(0, cursorCol);
                afterCursor = currentLine.substr(cursorCol);
                buffer.lines[cursorRow + topLine] = beforeCursor;
                buffer.lines.insert(buffer.lines.begin() + cursorRow + topLine + 1, afterCursor);
                cursorCol = 0;
                if (cursorRow < getScreenDimensions().second - 2) {
                    ++cursorRow;
                } else {
                    ++topLine;
                }
                break;
            case SpecialKey::Backspace:
                if (cursorCol > 0) {
                    buffer.deleteChar(cursorRow + topLine, cursorCol - 1);
                    --cursorCol;
                } else if (cursorRow + topLine > 0) {
                    int prevLineLength = buffer.lines[cursorRow + topLine - 1].length();
                    buffer.lines[cursorRow + topLine - 1] += buffer.lines[cursorRow + topLine];
                    buffer.lines.erase(buffer.lines.begin() + cursorRow + topLine);
                    if (cursorRow > 0) {
                        --cursorRow;
                    } else if (topLine > 0) {
                        --topLine;
                    }
                    cursorCol = prevLineLength;
                }
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