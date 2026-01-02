#ifndef LOGGER_H
#define LOGGER_H

#include <string>

void logProxy(const std::string& ip,
              const std::string& host,
              const std::string& port,
              const std::string& method,
              const std::string& path,
              const std::string& status,
              int bytes);
#endif