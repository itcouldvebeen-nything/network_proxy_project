#include <iostream>
#include <thread>
#include <iomanip>
#include <filesystem>
#include "../include/Common.h"
#include "../include/ProxyCore.h"
#include "../include/Filter.h"
#include "../include/Config.h"

namespace fs = std::filesystem;

SOCKET listenSock = INVALID_SOCKET;

BOOL WINAPI ctrl_handler(DWORD type) {
    if (type == CTRL_C_EVENT) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "[SHUTDOWN] Signal received. Cleaning up resources..." << std::endl;
        if (listenSock != INVALID_SOCKET) closesocket(listenSock);
        WSACleanup();
        std::cout << "[SHUTDOWN] Proxy Server halted safely." << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        exit(0);
    }
    return TRUE;
}

void printBanner(int port) {
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "         CUSTOM NETWORK PROXY SERVER v1.0" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << " [SYSTEM] Winsock Initialized ... OK" << std::endl;
    
    // Log Directory Logic
    std::string logDir = "logs";
    if (!fs::exists(logDir)) {
        fs::create_directory(logDir);
        std::cout << " [FS]     Created directory: /logs" << std::endl;
    } else {
        std::cout << " [FS]     Log directory found ... OK" << std::endl;
    }

    std::cout << " [CONFIG] Port: " << port << std::endl;
    std::cout << " [FILTER] Logic operational." << std::endl;
    std::cout << " [STATUS] Proxy is listening on 0.0.0.0:" << port << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << " [READY]  Waiting for client connections..." << std::endl;
    std::cout << " [HINT]   Press Ctrl+C to terminate the server." << std::endl;
    std::cout << std::string(60, '=') << std::endl << std::endl;
}

int main() {
    if (!Config::load("config/server.cfg")) {
        std::cerr << "[WARNING] Using defaults. Ensure config/server.cfg exists." << std::endl;
    }

    int port = Config::getInt("PORT", 8888);
    std::string filterPath = Config::getString("FILTER_PATH", "config/blocked.txt");

    loadFilters(filterPath);
    SetConsoleCtrlHandler(ctrl_handler, TRUE);

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[FATAL] Winsock startup failed." << std::endl;
        return 1;
    }

    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons((u_short)port);

    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR ||
        listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[FATAL] Could not bind to port " << port << ". Is it already in use?" << std::endl;
        WSACleanup();
        return 1;
    }

    printBanner(port);

    while (true) {
        SOCKET client = accept(listenSock, NULL, NULL);
        if (client != INVALID_SOCKET) {
            std::thread(handleClient, client).detach();
        }
    }

    WSACleanup();
    return 0;
}