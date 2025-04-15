#include "filesystem/pathUtils.hpp"

std::string getAbsolutePath(const std::string relPath) {
    try {
        // Get the current working directory and concatenate with the relative path
        std::filesystem::path absPath = std::filesystem::absolute(relPath);
        return absPath.string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error getting absolute path: " << e.what() << std::endl;
        return "";
    }
}

void normalizePath(std::string path) {
    try {
        // Normalize the path (removes redundant slashes, etc.)
        std::filesystem::path fsPath(path);
        fsPath = fsPath.lexically_normal();
        path = fsPath.string();  // Update the original path with the normalized path
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error normalizing path: " << e.what() << std::endl;
    }
}

bool fileExists(const std::string path) {
    try {
        // Check if the file exists at the specified path
        std::filesystem::path fsPath(path);
        return std::filesystem::exists(fsPath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error checking if file exists: " << e.what() << std::endl;
        return false;
    }
}