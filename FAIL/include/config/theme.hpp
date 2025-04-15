#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ui/colors.hpp"

void loadTheme(const std::string themeName);

Color getColor(const std::string elementName);