#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

void loadConfig(const std::string path);

void applyDefaultConfig();

std::string getConfigValue(const std::string& key);
