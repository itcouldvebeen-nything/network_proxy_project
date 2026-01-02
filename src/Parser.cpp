#include "../include/Parser.h"
#include <sstream>

int recvHeaders(SOCKET sock, std::string& outData) {
    char buffer[1024];
    while (true) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) return n;
        buffer[n] = '\0';
        outData += buffer;
        if (outData.find("\r\n\r\n") != std::string::npos) break;
        if (outData.length() > 8192) return -2; 
    }
    return (int)outData.length();
}

HttpRequest parseHttpRequest(const std::string& data) {
    HttpRequest req;
    req.raw = data;
    size_t firstLineEnd = data.find("\r\n");
    if (firstLineEnd == std::string::npos) return req;

    std::string firstLine = data.substr(0, firstLineEnd);
    std::istringstream iss(firstLine);
    iss >> req.method >> req.path >> req.version;

    size_t hostPos = data.find("Host: ");
    if (hostPos != std::string::npos) {
        size_t hostEnd = data.find("\r\n", hostPos);
        std::string hostLine = data.substr(hostPos + 6, hostEnd - (hostPos + 6));
        size_t colon = hostLine.find(':');
        if (colon != std::string::npos) {
            req.host = hostLine.substr(0, colon);
            req.port = hostLine.substr(colon + 1);
        } else {
            req.host = hostLine;
            req.port = "80";
        }
    }
    return req;
}

std::string modifyRequestLine(const HttpRequest& req) {
    // Reconstruct the request but force 'Connection: close'
    std::string newHeaders = req.method + " " + req.path + " " + req.version + "\r\n";
    
    std::string remainder = req.raw;
    size_t firstLineEnd = remainder.find("\r\n");
    if (firstLineEnd != std::string::npos) {
        remainder = remainder.substr(firstLineEnd + 2);
    }

    auto fixHeader = [](std::string& s, const std::string& key) {
        size_t pos = s.find(key + ":");
        if (pos != std::string::npos) {
            size_t end = s.find("\r\n", pos);
            s.replace(pos, end - pos, key + ": close");
        }
    };

    fixHeader(remainder, "Connection");
    fixHeader(remainder, "Proxy-Connection");

    return newHeaders + remainder;
}