#include "FileUtils.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FileUtils::readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("cannot open: " + path);
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

void FileUtils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot write: " + path);
    out.write(content.data(), content.size());
}
