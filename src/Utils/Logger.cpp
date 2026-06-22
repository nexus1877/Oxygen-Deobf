#include "Logger.hpp"
#include <iostream>
void Logger::info(const std::string& msg) { std::cout << "[+] " << msg << std::endl; }
void Logger::error(const std::string& msg) { std::cerr << "[-] " << msg << std::endl; }
