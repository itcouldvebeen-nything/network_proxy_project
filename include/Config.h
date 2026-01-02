#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
public:
    static bool load(const std::string& filename);
    static int getInt(const std::string& key, int defaultVal);
    static std::string getString(const std::string& key, const std::string& defaultVal);

private:
    static std::map<std::string, std::string> settings;
};

#endif