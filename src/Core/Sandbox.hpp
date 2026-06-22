#pragma once
#include "LuauVM.hpp"
#include <string>
#include <vector>
#include <unordered_map>

struct DeobfResult {
    std::string cleanSource;
    std::vector<std::string> warnings;
};

class Sandbox {
public:
    Sandbox();
    DeobfResult extractFunctions(const std::string& obfuscatedSource);
    DeobfResult extractAllDefinedFunctions(const std::string& obfuscatedSource);
private:
    LuauVM vm;
    void setupSafeEnvironment();
    void collectGlobalFunctions(std::vector<std::string>& out);
};
