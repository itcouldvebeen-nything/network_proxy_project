#include "../include/Filter.h"
#include <fstream>
#include <set>
#include <iostream>
#include <algorithm>
#include <mutex>

std::set<std::string> blockedDomains;
std::mutex filterMtx;

std::string normalize(std::string str) {
    if (str.empty()) return "";
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void loadFilters(const std::string& filename) {
    std::lock_guard<std::mutex> lock(filterMtx);
    std::ifstream file(filename);
    std::string line;
    
    blockedDomains.clear();
    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::string clean = normalize(line);
            if (!clean.empty()) blockedDomains.insert(clean);
        }
        std::cout << "[INIT] Filter list loaded: " << blockedDomains.size() << " domains." << std::endl;
    } else {
        std::cerr << "[ERROR] Could not find filter file: " << filename << std::endl;
    }
}

bool isBlocked(std::string host) {
    std::lock_guard<std::mutex> lock(filterMtx);
    host = normalize(host);
    
    if (blockedDomains.count(host)) return true;
    
    for (const auto& blocked : blockedDomains) {
        std::string suffix = "." + blocked;
        if (host.length() > suffix.length()) {
            if (host.compare(host.length() - suffix.length(), suffix.length(), suffix) == 0) {
                return true;
            }
        }
    }
    return false;
}