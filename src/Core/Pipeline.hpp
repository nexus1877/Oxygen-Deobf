#pragma once
#include <string>
#include <vector>

enum class Obfuscator { IronBrew, MoonSec, Prometheus, WeAreDevs, Unknown };

class Pipeline {
public:
    Pipeline();
    std::string process(const std::string& filename);
private:
    Obfuscator detectObfuscator(const std::string& source);
    std::string deobfuscate(Obfuscator type, const std::string& source);
};
