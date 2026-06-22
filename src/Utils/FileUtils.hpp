#pragma once
#include <string>
class FileUtils {
public:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
};
