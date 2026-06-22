#include "Core/Pipeline.hpp"
#include "Utils/Logger.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        Logger::error("usage: OxygenDeobf <file>");
        return 1;
    }
    try {
        Pipeline p;
        std::string result = p.process(argv[1]);
        std::cout << result;
    } catch (const std::exception& e) {
        Logger::error(e.what());
        return 1;
    }
    return 0;
}
