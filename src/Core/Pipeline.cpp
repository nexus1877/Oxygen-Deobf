#include "Pipeline.hpp"
#include "Sandbox.hpp"
#include "VariableRenamer.hpp"
#include "ErrorFixer.hpp"
#include "../Utils/FileUtils.hpp"
#include "../Utils/Logger.hpp"

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
    auto result = sb.extractFunctions(source);
    std::string clean = result.cleanSource.empty() ? source : result.cleanSource;

    clean = renameVariables(clean);
    clean = autoFixSource(clean);

    if (clean.empty()) clean = source;
    return clean;
}

std::string Pipeline::process(const std::string& filename) {
    std::string source = FileUtils::readFile(filename);
    Obfuscator type = detectObfuscator(source);
    Logger::info("Detected: " + std::to_string(static_cast<int>(type)));
    return deobfuscate(type, source);
}
