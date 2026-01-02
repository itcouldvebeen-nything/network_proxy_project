#ifndef LOGGER_H
#define LOGGER_H
#include <string>

// Change the 3rd parameter to std::string
void logProxy(const std::string& ip,
    const std::string& host,
    const std::string& port, // <--- Change this
    const std::string& method,
    const std::string& path,
    const std::string& status,
    int bytes);
#endif