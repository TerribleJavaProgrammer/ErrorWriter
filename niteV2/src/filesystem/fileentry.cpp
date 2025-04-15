#include "filesystem/fileEntry.hpp"

FileEntry::FileEntry(const std::string& filePath) {
    // Implement the contsructor to the FileEntry class
}

bool FileEntry::load() {
    // Load a string into the FileEntry
}

bool FileEntry::save() {
    // Save the FileEntry
}

std::string FileEntry::getFileName() const {
    // Return the file name
}

std::string FileEntry::getFilePath() const {
    // Return the file path
}

long FileEntry::getFileSize() const {
    // Return the file size
}

std::chrono::system_clock::time_point FileEntry::getLastModifiedTime() const {
    // Return the last modified time
}

std::string FileEntry::getContent() const {
    // Return the content of a FileEntry
}

void FileEntry::setContent(const std::string& content) {
    // Set the content of a FileEntry
}

void FileEntry::updateLastModifiedTime() {
    // Update the last modified time of a file entry
}