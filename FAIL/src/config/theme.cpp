#include "config/theme.hpp"

static std::unordered_map<std::string, Color> themeMap;

void loadTheme(const std::string themeName) {
    themeMap.clear();
    std::ifstream file("assets/" + themeName + ".nitetheme");
    if (!file) {
        std::cerr << "Could not open theme: " << themeName << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string element, rStr, gStr, bStr;
        if (std::getline(iss, element, '=') &&
            std::getline(iss, rStr, ',') &&
            std::getline(iss, gStr, ',') &&
            std::getline(iss, bStr)) {
            int r = std::stoi(rStr);
            int g = std::stoi(gStr);
            int b = std::stoi(bStr);
            themeMap[element] = {r, g, b};
        }
    }
}

Color getColor(const std::string elementName) {
    if (themeMap.count(elementName)) {
        return themeMap[elementName];
    }
    return {255, 255, 255};  // fallback: white
}