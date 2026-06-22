#include "Pipeline.hpp"
#include "Sandbox.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Utils/Logger.hpp"
#include "../Obfuscators/IronBrew.cpp"
#include "../Obfuscators/MoonSec.cpp"
#include "../Obfuscators/Prometheus.cpp"
#include "../Obfuscators/WeAreDevs.cpp"

Pipeline::Pipeline() {}

Obfuscator Pipeline::detectObfuscator(const std::string& source) {
    if (source.find("IronBrew") != std::string::npos || source.find("IB2") != std::string::npos)
        return Obfuscator::IronBrew;
    if (source.find("MoonSec") != std::string::npos)
        return Obfuscator::MoonSec;
    if (source.find("Prometheus") != std::string::npos || source.find("prothemus") != std::string::npos)
        return Obfuscator::Prometheus;
    if (source.find("WeAreDevs") != std::string::npos || source.find("wrd") != std::string::npos)
        return Obfuscator::WeAreDevs;
    return Obfuscator::Unknown;
}

std::string Pipeline::deobfuscate(Obfuscator type, const std::string& source) {
    Sandbox sb;
    switch (type) {
        case Obfuscator::IronBrew:   return IronBrew::deob(source);
        case Obfuscator::MoonSec:    return MoonSec::deob(source);
        case Obfuscator::Prometheus: return Prometheus::deob(source);
        case Obfuscator::WeAreDevs:  return WeAreDevs::deob(source);
        default: {
            auto result = sb.extractFunctions(source);
            if (result.cleanSource.empty()) return source;
            return result.cleanSource;
        }
    }
}

std::string Pipeline::process(const std::string& filename) {
    std::string source = FileUtils::readFile(filename);
    Obfuscator type = detectObfuscator(source);
    Logger::info("Detected: " + std::to_string(static_cast<int>(type)));
    return deobfuscate(type, source);
}
