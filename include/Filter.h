#ifndef FILTER_H
#define FILTER_H

#include <string>

void loadFilters(const std::string& filename); 
bool isBlocked(std::string host);

#endif