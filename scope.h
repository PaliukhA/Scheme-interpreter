#pragma once

#include <memory>
#include <map>
#include <vector>
#include "exeption.h"

class Object;

struct Scope {
    Scope();
    Scope(Scope *father);
    std::shared_ptr<Object> Get(std::string name);
    void Init(std::string name, std::shared_ptr<Object> val) {
        table[name] = val;
    }
    void Set(std::string name, std::shared_ptr<Object> val);
    std::map<std::string, std::shared_ptr<Object>> table;
    Scope *father;
};
