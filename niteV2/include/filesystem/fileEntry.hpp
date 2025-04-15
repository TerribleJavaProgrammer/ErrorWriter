#ifndef FILEENTRY_HPP
#define FILEENTRY_HPP

#include <string>
#include <chrono>
#include <fstream>
#include <iostream>

class FileEntry {
    public:
        // Constructor to create a FileEntry with a specific file path
        FileEntry(const std::string& filePath);

        // Load file content into a string
        bool load();

        // Save the current content to the file
        bool save();

        // Get file name (just the name without the directory)
        std::string getFileName() const;

        // Get file path (full path)
        std::string getFilePath() const;

        // Get file size in bytes
        long getFileSize() const;

        // Get the last modified time of the file
        std::chrono::system_clock::time_point getLastModifiedTime() const;

        // Get the file content (to work with the text in the editor)
        std::string getContent() const;

        // Set new content to the file (this will be used for saving)
        void setContent(const std::string& content);

    private:
        std::string filePath;             // Full path to the file
        std::string content;              // Content of the file
        std::chrono::system_clock::time_point lastModified;  // Last modified timestamp

        // Helper function to update last modified time
        void updateLastModifiedTime();
};

#endif // FILEENTRY_HPP