#include "../include/ProxyCore.h"
#include "../include/Parser.h"
#include "../include/Filter.h"
#include "../include/Logger.h"
#include <iostream>
#include <mutex>

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

SOCKET connectToRemote(const std::string& host, const std::string& port) {
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) return INVALID_SOCKET;
    
    SOCKET s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s != INVALID_SOCKET) {
        if (connect(s, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
            closesocket(s);
            s = INVALID_SOCKET;
        }
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
    if (recvHeaders(clientSocket, rawData) <= 0) {
        closesocket(clientSocket); 
        return;
    }

    HttpRequest req = parseHttpRequest(rawData);
    if (req.host.empty()) {
        closesocket(clientSocket);
        return;
    }

    if (isBlocked(req.host)) {
        sendAll(clientSocket, HTTP_403.c_str(), (int)HTTP_403.length());
        logProxy(ipStr, req.host, req.port, req.method, req.path, "BLOCKED", 0);
        closesocket(clientSocket);
        return;
    }

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

    closesocket(remoteSocket);
    closesocket(clientSocket);
}