#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

struct HttpRequest {
    std::string method;
    std::string host;
    std::string port = "80"; 
    std::string path;
    std::string version;
    std::string raw;
};

const std::string HTTP_403 = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nAccess Denied: Domain is blocked.";
const std::string HTTP_200_CON = "HTTP/1.1 200 Connection Established\r\n\r\n";
const std::string HTTP_502 = "HTTP/1.1 502 Bad Gateway\r\nConnection: close\r\n\r\n";

#endif