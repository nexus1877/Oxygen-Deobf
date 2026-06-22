#include "VariableRenamer.hpp"
#include <Luau/Parser.h>
#include <Luau/Ast.h>
#include <Luau/Common.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

struct RenameState {
    std::unordered_map<std::string, std::string> renames;
    int counter = 1;
};

static void collectRenames(Luau::AstStatBlock* block, RenameState& state) {
    for (Luau::AstStat* stat : block->body) {
        if (auto* local = stat->as<Luau::AstStatLocal>()) {
            for (Luau::AstLocal* var : local->vars) {
                std::string oldName = var->name.value;
                if (state.renames.find(oldName) == state.renames.end()) {
                    state.renames[oldName] = "var_" + std::to_string(state.counter++);
                }
            }
        }
        else if (auto* func = stat->as<Luau::AstStatFunction>()) {
            if (auto* name = func->name->as<Luau::AstExprLocal>()) {
                std::string oldName = name->local->name.value;
                if (state.renames.find(oldName) == state.renames.end()) {
                    state.renames[oldName] = "func_" + std::to_string(state.counter++);
                }
            }
        }
    }
}

static std::string applyRenames(const std::string& source, const std::unordered_map<std::string, std::string>& renames) {
    std::string result = source;
    std::vector<std::pair<std::string, std::string>> sorted(renames.begin(), renames.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.first.size() > b.first.size(); });

    for (const auto& [old, nw] : sorted) {
        size_t pos = 0;
        while ((pos = result.find(old, pos)) != std::string::npos) {
            bool leftOk = (pos == 0 || (!std::isalnum(result[pos-1]) && result[pos-1] != '_'));
            bool rightOk = (pos + old.size() >= result.size() || (!std::isalnum(result[pos + old.size()]) && result[pos + old.size()] != '_'));
            if (leftOk && rightOk) {
                result.replace(pos, old.size(), nw);
                pos += nw.size();
            } else {
                pos += old.size();
            }
        }
    }
    return result;
}

std::string renameVariables(const std::string& source) {
    Luau::AstNameTable names;
    Luau::Allocator allocator;
    Luau::ParseOptions options;
    Luau::ParseResult parseResult = Luau::Parser::parse(source.c_str(), source.size(), names, allocator, options);
    if (!parseResult.root) return source;

    Luau::AstStatBlock* root = parseResult.root->as<Luau::AstStatBlock>();
    if (!root) return source;

    RenameState state;
    collectRenames(root, state);

    return applyRenames(source, state.renames);
}
