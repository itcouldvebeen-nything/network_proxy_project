#include "../include/Config.h"
#include <fstream>
#include <sstream>

std::map<std::string, std::string> Config::settings;

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; 
        std::istringstream is_line(line);
        std::string key, value;
        if (std::getline(is_line, key, '=') && std::getline(is_line, value)) {
            Config::settings[key] = value;
        }
    }
    return true;
}

int Config::getInt(const std::string& key, int defaultVal) {
    try {
        return settings.count(key) ? std::stoi(settings[key]) : defaultVal;
    } catch (...) { return defaultVal; }
}

std::string Config::getString(const std::string& key, const std::string& defaultVal) {
    return settings.count(key) ? settings[key] : defaultVal;
}