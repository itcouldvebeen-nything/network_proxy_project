#include "Parser.h"
#include <iostream>
#include <vector>
#include "Common.h"
#include <sstream>

int recvHeaders(SOCKET sock, std::string& outData) {
    char buffer[1024];
    while (true) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) return n;
        buffer[n] = '\0';
        outData += buffer;
        if (outData.find("\r\n\r\n") != std::string::npos) break;
        if (outData.length() > 8192) return -2; // Header too large [cite: 109, 110]
    }
    return (int)outData.length();
}

HttpRequest parseHttpRequest(const std::string& data) {
    HttpRequest req;
    req.raw = data;
    size_t firstLineEnd = data.find("\r\n");
    if (firstLineEnd == std::string::npos) return req;

    std::string firstLine = data.substr(0, firstLineEnd);
    size_t s1 = firstLine.find(' ');
    size_t s2 = firstLine.find(' ', s1 + 1);
    
    if (s1 != std::string::npos && s2 != std::string::npos) {
        req.method = firstLine.substr(0, s1);
        req.path = firstLine.substr(s1 + 1, s2 - s1 - 1);
        req.version = firstLine.substr(s2 + 1);
    }

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
            req.port = "80"; // Default HTTP port [cite: 4, 65]
        }
    }
    return req;
}

std::string modifyRequestLine(const HttpRequest& req) {
    // Forces "Connection: close" to disable keep-alive [cite: 111]
    std::string newHeaders = req.method + " " + req.path + " " + req.version + "\r\n";
    std::string body = req.raw;
    size_t firstLineEnd = body.find("\r\n");
    if (firstLineEnd != std::string::npos) {
        body = body.substr(firstLineEnd + 2);
    }

    auto replaceHeader = [](std::string& s, const std::string& key) {
        size_t pos = s.find(key);
        if (pos != std::string::npos) {
            size_t end = s.find("\r\n", pos);
            s.replace(pos, end - pos, key + ": close");
        } else {
            s += key + ": close\r\n";
        }
    };

    replaceHeader(body, "Connection");
    replaceHeader(body, "Proxy-Connection");
    return newHeaders + body;
}