#include "buffer.hpp"

Buffer::Buffer() {
    // Write a constructor for the buffer class
}

void Buffer::initBuffer() {
    // Write method to initialize buffer
}

void Buffer::insertChar(int row, int col, char c) {
    // Write method to insert a character given a row, col, and char
}

void Buffer::deleteChar(int row, int col) {
    // Write a method to delete a character given a row and col
}

std::string Buffer::getLine(int row) {
    // Write a method to return a line given a buffer and a row
}

void Buffer::loadFile(const std::string& path) {
    // Write a method to load a file into the buffer provided given a path
} 

void Buffer::saveFile(const std::string& path) {
    // Write a method to save a buffer to the file at the given path
}

BufferMode Buffer::getMode() {
    // Write a method to get the mode of the buffer
}

void Buffer::setMode(BufferMode mode) {
    // Write a method to set the mode of the buffer
}
