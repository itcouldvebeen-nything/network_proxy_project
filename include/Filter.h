#ifndef FILTER_H
#define FILTER_H

#include <string>

// Ensure these match the .cpp exactly!
void loadFilters(const std::string& filename); 
bool isBlocked(std::string host);

#endif