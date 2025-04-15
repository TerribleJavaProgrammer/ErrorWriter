#include "filesystem/fileEntry.hpp"
#include <filesystem>
#include <chrono>
#include <fstream>

FileEntry::FileEntry(const std::string& filePath) : filePath(filePath) {
    // Optionally initialize content with empty string, or load content if desired
    load();
}

bool FileEntry::load() {
    // Check if the file exists
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "File not found: " << filePath << std::endl;
        return false;
    }

    // Open file to read content
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    // Read the file content into the 'content' variable
    content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Get the last modified time and convert it to system_clock::time_point
    auto fileTime = std::filesystem::last_write_time(filePath);

    // Convert file_time_type to system_clock::time_point
    lastModified = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

    return true;
}

bool FileEntry::save() {
    // Open file to save content
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return false;
    }

    // Write content to file
    file << content;

    // Update the last modified time after saving
    updateLastModifiedTime();

    return true;
}

std::string FileEntry::getFileName() const {
    return std::filesystem::path(filePath).filename().string();
}

std::string FileEntry::getFilePath() const {
    return filePath;
}

long FileEntry::getFileSize() const {
    return std::filesystem::file_size(filePath);
}

std::chrono::system_clock::time_point FileEntry::getLastModifiedTime() const {
    return lastModified;
}

std::string FileEntry::getContent() const {
    return content;
}

void FileEntry::setContent(const std::string& newContent) {
    content = newContent;
}

void FileEntry::updateLastModifiedTime() {
    // Convert string file path to filesystem path
    std::filesystem::path path(filePath);

    // Get the current time (system clock)
    auto systemTime = std::chrono::system_clock::now();

    // Convert system_clock time to file_time_type
    auto fileTime = std::filesystem::file_time_type::clock::now();

    // Set the last modified time of the file
    std::filesystem::last_write_time(path, fileTime);

    // Update the internal last modified time
    lastModified = systemTime;
}