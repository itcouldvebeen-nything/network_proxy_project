#ifndef PARSER_H
#define PARSER_H

#include "Common.h"
#include <string>

int recvHeaders(SOCKET sock, std::string& outData);
HttpRequest parseHttpRequest(const std::string& data);
std::string modifyRequestLine(const HttpRequest& req);

#endif