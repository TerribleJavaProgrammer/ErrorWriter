#ifndef PATHUTILS_HPP
#define PATHUTILS_HPP
#include <string>

std::string getAbsolutePath(const std::string relPath);

void normalizePath(std::string path);

bool fileExists(const std::string path);

#endif // PATHUTILS_HPP
