#include "Common.h"     // Must be first! Defines SOCKET and HttpRequest
#include "Parser.h"     // Defines recvHeaders, parseHttpRequest
#include "Filter.h"     // Defines isBlocked
#include "Logger.h"     // Defines logProxy
#include "ProxyCore.h"  // Your core header
#include <iostream>
#include <mutex>
#include <ws2tcpip.h>   // Added for inet_ntop and addrinfo

std::mutex coutMtx; 

int sendAll(SOCKET s, const char* buf, int len) {
    int totalSent = 0;
    while (totalSent < len) {
        int sent = send(s, buf + totalSent, len - totalSent, 0);
        if (sent <= 0) return SOCKET_ERROR;
        totalSent += sent;
    }
    return totalSent;
}

// CHANGED: Accept port as std::string to match HttpRequest.port
SOCKET connectToRemote(const std::string& host, const std::string& port) {
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    // No more std::to_string needed
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) return INVALID_SOCKET;
    
    SOCKET s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s != INVALID_SOCKET && connect(s, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        closesocket(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(res);
    return s;
}

void setSocketTimeout(SOCKET s, int milliseconds) {
    DWORD timeout = milliseconds;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}

void handleClient(SOCKET clientSocket) {
    setSocketTimeout(clientSocket, 7000); 

    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    char ipStr[INET_ADDRSTRLEN] = "Unknown";
    if (getpeername(clientSocket, (sockaddr*)&clientAddr, &addrLen) == 0) {
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
    }

    std::string rawData;
    // recvHeaders is now FOUND because of #include "Parser.h"
    int headerBytes = recvHeaders(clientSocket, rawData);
    if (headerBytes <= 0) {
        closesocket(clientSocket); 
        return;
    }

    HttpRequest req = parseHttpRequest(rawData);
    if (req.host.empty()) {
        closesocket(clientSocket);
        return;
    }

    // isBlocked is now FOUND because of #include "Filter.h"
    if (isBlocked(req.host)) {
        {
            std::lock_guard<std::mutex> lock(coutMtx);
            std::cout << "[BLOCKED] " << req.host << std::endl;
        }
        sendAll(clientSocket, HTTP_403.c_str(), (int)HTTP_403.length());
        logProxy(ipStr, req.host, req.port, req.method, req.path, "BLOCKED", 0);
        closesocket(clientSocket);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(coutMtx);
        std::cout << "[FORWARD] " << req.method << " " << req.host << std::endl;
    }

    // Fixed the string/int mismatch here
    SOCKET remoteSocket = connectToRemote(req.host, req.port);
    if (remoteSocket == INVALID_SOCKET) {
        std::string err = "HTTP/1.1 502 Bad Gateway\r\nConnection: close\r\n\r\n";
        sendAll(clientSocket, err.c_str(), (int)err.length());
        closesocket(clientSocket); 
        return;
    }

    setSocketTimeout(remoteSocket, 10000); 

    std::string finalRequest = modifyRequestLine(req); 
    sendAll(remoteSocket, finalRequest.c_str(), (int)finalRequest.length());

    char buffer[16384];
    int n, totalBytes = 0;
    while ((n = recv(remoteSocket, buffer, sizeof(buffer), 0)) > 0) {
        if (sendAll(clientSocket, buffer, n) == SOCKET_ERROR) break;
        totalBytes += n;
    }

    logProxy(ipStr, req.host, req.port, req.method, req.path, "ALLOWED", totalBytes);

    {
        std::lock_guard<std::mutex> lock(coutMtx);
        std::cout << "[DONE] " << req.host << " (" << totalBytes << " bytes)" << std::endl;
    }

    closesocket(remoteSocket);
    closesocket(clientSocket);
}