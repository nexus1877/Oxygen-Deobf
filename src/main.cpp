#include "Core/Pipeline.hpp"
#include "Utils/Logger.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        std::string command = argv[1];
        if (command == "--update" || command == "/update") {
            Logger::info("Updating from repository...");
            int ret = system("bash update.sh");
            if (ret == 0) Logger::info("Update successful, restart the tool.");
            else Logger::error("Update failed. Check your connection and project folder.");
            return ret;
        }
    }

    if (argc < 2) {
        Logger::error("usage: OxygenDeobf <file>   or   OxygenDeobf --update");
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
