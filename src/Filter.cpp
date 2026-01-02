#include "Filter.h"
#include "Common.h"
#include <fstream>
#include <string>
#include <set>
#include <iostream>
#include <algorithm>
#include <mutex> // Added for thread-safety

std::set<std::string> blockedDomains;
std::mutex filterMtx; // Prevents crashes if multiple threads check at once

std::string normalize(std::string str) {
    if (str.empty()) return "";
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// CHANGED: Added the filename parameter to match the header
void loadFilters(const std::string& filename) {
    std::lock_guard<std::mutex> lock(filterMtx);
    std::ifstream file(filename);
    std::string line;
    
    blockedDomains.clear(); // Clear existing if reloading

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::string clean = normalize(line);
            if (!clean.empty()) blockedDomains.insert(clean);
        }
        std::cout << "[INIT] Filter list loaded from " << filename 
                  << " (" << blockedDomains.size() << " domains)." << std::endl;
    } else {
        std::cerr << "[ERROR] Could not find " << filename << "!" << std::endl;
    }
}

bool isBlocked(std::string host) {
    std::lock_guard<std::mutex> lock(filterMtx);
    host = normalize(host);
    
    // 1. Exact Match
    if (blockedDomains.count(host)) return true;
    
    // 2. Subdomain Match (e.g., ad.doubleclick.net matches doubleclick.net)
    for (const auto& blocked : blockedDomains) {
        if (host.length() > blocked.length()) {
            std::string suffix = "." + blocked;
            if (host.substr(host.length() - suffix.length()) == suffix) {
                return true;
            }
        }
    }
    return false;
}