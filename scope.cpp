#include "scope.h"

Scope::Scope() : table(), father(nullptr) {
}

Scope::Scope(Scope *father) : table(), father(father) {
}

std::shared_ptr<Object> Scope::Get(std::string name) {
    if (table.find(name) != table.end()) {
        return table[name];
    } else {
        if (father == nullptr) {
            throw NameError{};
        } else {
            return father->Get(name);
        }
    }
}

void Scope::Set(std::string name, std::shared_ptr<Object> val) {
    if (table.find(name) != table.end()) {
        table[name] = val;
    } else if (father == nullptr) {
        throw NameError{};
    } else {
        father->Set(name, val);
    }
}
