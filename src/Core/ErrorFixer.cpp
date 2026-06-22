#include "ErrorFixer.hpp"
#include <Luau/Compiler.h>
#include <stdexcept>
#include <regex>
#include <sstream>

static bool compileSucceeds(const std::string& code) {
    try {
        Luau::compile(code);
        return true;
    } catch (...) {
        return false;
    }
}

static std::string tryFixMissingEnd(const std::string& source) {
    std::string fixed = source;
    fixed += "\nend";
    if (compileSucceeds(fixed)) return fixed;
    return source;
}

static std::string removeProblematicLine(const std::string& source, int lineNum) {
    std::istringstream iss(source);
    std::ostringstream oss;
    std::string line;
    int current = 1;
    while (std::getline(iss, line)) {
        if (current != lineNum) oss << line << "\n";
        current++;
    }
    return oss.str();
}

std::string autoFixSource(const std::string& source) {
    if (compileSucceeds(source)) return source;

    std::string fixed = tryFixMissingEnd(source);
    if (compileSucceeds(fixed)) return fixed;

    try {
        Luau::compile(source);
    } catch (const std::exception& e) {
        std::string err = e.what();
        std::regex lineR("at line (\\d+)");
        std::smatch match;
        if (std::regex_search(err, match, lineR)) {
            int line = std::stoi(match[1].str());
            std::string cleaned = removeProblematicLine(source, line);
            if (compileSucceeds(cleaned)) return cleaned;
        }
    }

    return source;
}
