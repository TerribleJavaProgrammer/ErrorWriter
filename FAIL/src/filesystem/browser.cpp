#include "filesystem/browser.hpp"

FileBrowser::FileBrowser(const std::string& rootPath) : currentPath(rootPath) {}

std::vector<FileEntry> FileBrowser::listFiles() {
    std::vector<FileEntry> fileList;
    
    for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
        fileList.push_back({entry.path().filename().string(), entry.is_directory()});
    }
    
    return fileList;
}

void FileBrowser::changeDirectory(const std::string& dir) {
    std::filesystem::path newPath = std::filesystem::path(currentPath) / dir;
    
    if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath)) {
        currentPath = newPath.string();
    } else {
        std::cout << "Invalid directory" << std::endl;
    }
}

std::string FileBrowser::getCurrentDirectory() const {
    return currentPath;
}