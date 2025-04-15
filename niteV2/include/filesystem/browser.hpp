#ifndef BROWSER_HPP
#define BROWSER_HPP
#include <string>
#include "fileEntry.hpp"

void listDirectory(const std::string path, FileEntry& out, int& count);

void changeDirectory(const std::string path);

bool isDirectory(const std::string path);

#endif // BROWSER_HPP
