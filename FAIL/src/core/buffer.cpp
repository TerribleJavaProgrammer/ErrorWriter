#include "core/buffer.hpp"

Buffer::Buffer() {
    initBuffer();
}

void Buffer::initBuffer() {
    lines.clear();
    lines.push_back("");  // Start with one empty line
    mode = BufferMode::Normal;
}

void Buffer::insertChar(int row, int col, char c) {
    if (row < 0 || row >= (int)lines.size()) return;

    std::string& line = lines[row];
    if (col < 0) col = 0;
    if (col > (int)line.length()) col = line.length();

    line.insert(line.begin() + col, c);
}

void Buffer::deleteChar(int row, int col) {
    if (row < 0 || row >= (int)lines.size()) return;

    std::string& line = lines[row];
    if (col < 0 || col >= (int)line.length()) return;

    line.erase(line.begin() + col);
}

std::string Buffer::getLine(int row) {
    if (row < 0 || row >= (int)lines.size()) return "";
    return lines[row];
}

void Buffer::loadFile(const std::string& path) {
    std::ifstream file(path);
    lines.clear();

    if (!file) {
        lines.push_back("");  // If file can't be opened, start with empty buffer
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    if (lines.empty()) {
        lines.push_back("");  // Ensure at least one line exists
    }
}

void Buffer::saveFile(const std::string& path) {
    std::ofstream file(path);
    if (!file) return;

    for (size_t i = 0; i < lines.size(); ++i) {
        file << lines[i];
        if (i != lines.size() - 1) file << '\n';
    }
}

BufferMode Buffer::getMode() {
    return mode;
}

void Buffer::setMode(BufferMode newMode) {
    mode = newMode;
}