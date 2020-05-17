#pragma once

#include "assemble.h"

std::string IntToString(int val) {
    if (val == 0) {
        return "0";
    }
    std::string result;
    if (val < 0) {
        result = "-";
        val = -val;
    }
    std::vector<char> res;
    while (val > 0) {
        res.push_back(char(val % 10 + '0'));
        val /= 10;
    }
    for (int i = res.size(); i > 0; --i) {
        result += res[i - 1];
    }
    return result;
}

std::string Assemble(std::shared_ptr<Object> obj) {
    if (obj == nullptr) {
        return "()";
    }
    if (IsSymbol(obj)) {
        return AsSymbol(obj)->GetName();
    }
    if (IsNumber(obj)) {
        int result = AsNumber(obj)->GetValue();
        return IntToString(result);
    }
    if (IsBool(obj)) {
        bool result = AsBool(obj)->Get();
        if (result) {
            return "#t";
        } else {
            return "#f";
        }
    }
    if (IsCell(obj)) {
        std::string result = "(";
        while (IsCell(obj)) {
            auto cell = AsCell(obj);
            result += Assemble(cell->GetFirst());
            obj = cell->GetSecond();
            if (IsCell(obj)) {
                result += " ";
            }
        }
        if (obj != nullptr) {
            result += " . ";
            result += Assemble(obj);
        }
        result += ")";
        return result;
    }
    return "";
}
