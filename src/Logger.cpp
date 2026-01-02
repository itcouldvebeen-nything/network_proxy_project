#include "Logger.h"
#include <iostream>
#include <fstream>
#include <mutex>

std::mutex logMtx;

void logProxy(const std::string& ip, const std::string& host, const std::string& port,
    const std::string& method, const std::string& path, const std::string& status, int bytes) {
    std::lock_guard<std::mutex> lock(logMtx);

    // Example format: [IP] Method Host:Port Path STATUS (Bytes)
    std::cout << "[" << ip << "] " << method << " " << host << ":" << port
        << " " << path << " -> " << status << " (" << bytes << " bytes)" << std::endl;

    // Optional: Log to a file
    std::ofstream logFile("proxy.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << ip << "," << host << "," << port << "," << method << "," << status << "," << bytes << "\n";
    }
}