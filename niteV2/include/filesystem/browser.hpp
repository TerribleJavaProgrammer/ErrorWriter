#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

// Represents a file entry (could be a directory or file)
struct FileEntry {
    std::string name;   // Name of the file or directory
    bool isDirectory;   // True if it's a directory, false if it's a file
};

// Class responsible for file browsing
class FileBrowser {
public:
    FileBrowser(const std::string& rootPath); // Constructor with the root path for browsing

    // List all files and directories in the current directory
    std::vector<FileEntry> listFiles();

    // Change the current directory
    void changeDirectory(const std::string& dir);

    // Get the current directory
    std::string getCurrentDirectory() const;

private:
    std::string currentPath; // Current directory path
};