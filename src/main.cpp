#include <iostream>
#include <thread>
#include "../include/Common.h"
#include "../include/ProxyCore.h"
#include "../include/Filter.h"
#include "../include/Config.h"

SOCKET listenSock = INVALID_SOCKET;

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
    if (!Config::load("config/server.cfg")) {
        std::cerr << "[WARNING] Could not load config/server.cfg. Using defaults." << std::endl;
    }

    int port = Config::getInt("PORT", 8888);
    std::string filterPath = Config::getString("FILTER_PATH", "config/blocked.txt");

    loadFilters(filterPath);
    SetConsoleCtrlHandler(ctrl_handler, TRUE);

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons((u_short)port);

    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR ||
        listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Setup failed." << std::endl;
        return 1;
    }

    std::cout << "Proxy Server running on port " << port << std::endl;

    while (true) {
        SOCKET client = accept(listenSock, NULL, NULL);
        if (client != INVALID_SOCKET) {
            std::thread(handleClient, client).detach();
        }
    }

    WSACleanup();
    return 0;
}