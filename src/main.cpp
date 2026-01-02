#include <iostream>
#include <thread>
#include "Common.h"
#include "ProxyCore.h"
#include "Filter.h"

SOCKET listenSock = INVALID_SOCKET;

// Handles graceful shutdown when you press Ctrl+C
BOOL WINAPI ctrl_handler(DWORD type) {
    if (type == CTRL_C_EVENT) {
        std::cout << "\n[SHUTDOWN] Closing proxy server..." << std::endl;
        if (listenSock != INVALID_SOCKET) closesocket(listenSock);
        WSACleanup();
        exit(0);
    }
    return TRUE;
}

int main() {
    loadFilters("blocked.txt");
    SetConsoleCtrlHandler(ctrl_handler, TRUE);

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[ERROR] WinSock init failed." << std::endl;
        return 1;
    }

    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "[ERROR] Socket creation failed." << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces
    serverAddr.sin_port = htons(8888);       // PORT 8888

    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Bind failed. Is port 8888 already in use?" << std::endl;
        closesocket(listenSock);
        return 1;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Listen failed." << std::endl;
        return 1;
    }

    std::cout << "============================================" << std::endl;
    std::cout << "  PROXY SERVER STARTING ON PORT 8888        " << std::endl;
    std::cout << "  Press Ctrl+C to stop                      " << std::endl;
    std::cout << "============================================" << std::endl;

    while (true) {
        SOCKET client = accept(listenSock, NULL, NULL);
        if (client != INVALID_SOCKET) {
            // This starts the logic you wrote in ProxyCore.cpp
            std::thread(handleClient, client).detach();
        }
    }

    WSACleanup();
    return 0;
}