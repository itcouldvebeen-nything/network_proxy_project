#ifndef PROXYCORE_H
#define PROXYCORE_H

#include "Common.h"

void handleClient(SOCKET clientSocket);
SOCKET connectToRemote(const std::string& host, const std::string& port); 
int sendAll(SOCKET s, const char* buf, int len);
void setSocketTimeout(SOCKET s, int milliseconds);

#endif