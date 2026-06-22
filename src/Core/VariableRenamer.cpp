#include "VariableRenamer.hpp"
#include <Luau/Parser.h>
#include <Luau/Ast.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>

struct UsageStats {
    bool asCall = false;
    bool asTable = false;
    bool asString = false;
    bool asNumber = false;
    bool asLoop = false;
};

struct VarInfo {
    Luau::AstLocal* decl = nullptr;
    UsageStats stats;
};

class RenameVisitor : public Luau::AstVisitor {
public:
    std::unordered_map<std::string, VarInfo> locals;

    bool visit(Luau::AstExprLocal* node) override {
        auto it = locals.find(node->local->name.value);
        if (it != locals.end()) {
            Luau::AstNode* parent = node->parent;
            if (parent && parent->is<Luau::AstExprCall>()) {
                it->second.stats.asCall = true;
            }
        }
        return true;
    }

    bool visit(Luau::AstStatAssign* node) override {
        for (size_t i = 0; i < node->vars.size(); ++i) {
            if (auto* loc = node->vars[i]->as<Luau::AstExprLocal>()) {
                auto it = locals.find(loc->local->name.value);
                if (it != locals.end() && i < node->values.size()) {
                    if (node->values[i]->is<Luau::AstExprTable>()) {
                        it->second.stats.asTable = true;
                    } else if (node->values[i]->is<Luau::AstExprConstantString>()) {
                        it->second.stats.asString = true;
                    } else if (node->values[i]->is<Luau::AstExprConstantNumber>()) {
                        it->second.stats.asNumber = true;
                    }
                }
            }
        }
        return true;
    }

    bool visit(Luau::AstStatFor* node) override {
        if (auto* loc = node->var->as<Luau::AstExprLocal>()) {
            auto it = locals.find(loc->local->name.value);
            if (it != locals.end()) it->second.stats.asLoop = true;
        }
        return true;
    }

    bool visit(Luau::AstStatForIn* node) override {
        for (auto& var : node->vars) {
            if (auto* loc = var->as<Luau::AstExprLocal>()) {
                auto it = locals.find(loc->local->name.value);
                if (it != locals.end()) it->second.stats.asLoop = true;
            }
        }
        return true;
    }

    void collectDeclarations(Luau::AstStatBlock* root) {
        for (Luau::AstStat* stat : root->body) {
            if (auto* local = stat->as<Luau::AstStatLocal>()) {
                for (auto& var : local->vars) {
                    if (auto* lvar = var->as<Luau::AstLocal>()) {
                        locals[lvar->name.value] = { lvar, {} };
                    }
                }
            }
            if (auto* func = stat->as<Luau::AstStatFunction>()) {
                if (auto* name = func->name->as<Luau::AstExprLocal>()) {
                    locals[name->local->name.value] = { name->local, {} };
                }
            }
        }
    }
};

static std::string generateName(const std::string& base, const UsageStats& stats) {
    if (stats.asCall) return "func_" + base;
    if (stats.asTable) return "tbl_" + base;
    if (stats.asString) return "str_" + base;
    if (stats.asNumber) return "num_" + base;
    if (stats.asLoop) return "iter_" + base;
    return "var_" + base;
}

static std::string replaceAll(std::string source, const std::string& oldWord, const std::string& newWord) {
    std::string result;
    result.reserve(source.size() * 1.1);
    size_t pos = 0;
    while (pos < source.size()) {
        size_t found = source.find(oldWord, pos);
        if (found == std::string::npos) {
            result.append(source, pos, std::string::npos);
            break;
        }
        result.append(source, pos, found - pos);
        size_t after = found + oldWord.size();
        bool leftOk = (found == 0 || (!std::isalnum(source[found-1]) && source[found-1] != '_'));
        bool rightOk = (after >= source.size() || (!std::isalnum(source[after]) && source[after] != '_'));
        if (leftOk && rightOk) {
            result.append(newWord);
        } else {
            result.append(oldWord);
        }
        pos = after;
    }
    return result;
}

std::string renameVariables(const std::string& source) {
    Luau::ParseResult parseResult = Luau::Parser::parse(source.c_str(), source.size());
    if (!parseResult.root) return source;

    Luau::AstStatBlock* root = parseResult.root->as<Luau::AstStatBlock>();
    if (!root) return source;

    RenameVisitor visitor;
    visitor.collectDeclarations(root);
    root->visit(&visitor);

    std::vector<std::pair<std::string, std::string>> replacements;
    int counter = 1;
    for (auto& [name, info] : visitor.locals) {
        std::string newName = generateName(std::to_string(counter++), info.stats);
        replacements.emplace_back(name, newName);
    }

    std::sort(replacements.begin(), replacements.end(),
              [](const auto& a, const auto& b) { return a.first.size() > b.first.size(); });

    std::string result = source;
    for (const auto& [old, nw] : replacements) {
        result = replaceAll(result, old, nw);
    }
    return result;
}
