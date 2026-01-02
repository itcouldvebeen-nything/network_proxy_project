#include "../include/Logger.h"
#include "../include/Config.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>


std::mutex logMtx;

std::string getTimestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void logProxy(const std::string& ip, const std::string& host, const std::string& port,
              const std::string& method, const std::string& path, const std::string& status, int bytes) {
    std::lock_guard<std::mutex> lock(logMtx);
    std::string ts = getTimestamp();
    
    // Use the config value or default
    std::string logFilePath = Config::getString("LOG_PATH", "proxy.log");

    std::ofstream logFile(logFilePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << ts << "," << ip << "," << host << "," << method << "," << status << "," << bytes << std::endl; 
        logFile.close(); // Closing forces the OS to write to disk immediately
    } else {
        // If it fails, print to console so you know why
        std::cerr << "[ERROR] Could not write to log file: " << logFilePath << std::endl;
    }

    // Console output stays the same
    std::cout << "[" << ts << "] [" << ip << "] " << std::left << std::setw(8) << method 
              << host << ":" << port << " -> " << status << " (" << bytes << " bytes)" << std::endl;
}