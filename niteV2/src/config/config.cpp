#include "config/config.hpp"

static std::unordered_map<std::string, std::string> configValues;

void loadConfig(const std::string path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open config file: " << path << "\n";
        applyDefaultConfig();
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            configValues[key] = value;
        }
    }
}

void applyDefaultConfig() {
    configValues["theme"] = "default";
    configValues["tabSize"] = "4";
    configValues["showLineNumbers"] = "true";
}

std::string getConfigValue(const std::string& key) {
    return configValues.count(key) ? configValues[key] : "";
}