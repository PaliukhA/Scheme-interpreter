#pragma once

#include "object.h"
#include <sstream>
#include <vector>
#include "assemble.h"

class Compilation {
public:
    Compilation() : scopes() {
        scope = new Scope();
        scopes.push_back(scope);
    }
    std::string Build(const std::string &right) {
        std::stringstream ss{right};
        Tokenizer token{&ss};
        std::shared_ptr<Object> root = Read(&token);
        std::shared_ptr<Object> result = root->Eval(scope, scopes);
        return Assemble(result);
    }
    ~Compilation() {
        for (auto sc : scopes) {
            delete sc;
        }
    }
    Scope *scope;
    std::vector<Scope *> scopes;
};