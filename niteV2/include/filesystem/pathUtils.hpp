#pragma once

#include <filesystem>
#include <iostream>
#include <string>

std::string getAbsolutePath(const std::string relPath);

void normalizePath(std::string path);

bool fileExists(const std::string path);
