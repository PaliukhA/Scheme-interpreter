#pragma once

#include <memory>
#include <vector>
#include "tokenizer.h"
#include "scope.h"

struct Scope;

class Object {
public:
    explicit Object(int type) : type_(type) {
    }
    int type_;
    virtual ~Object() = default;
    virtual std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &) {
        return nullptr;
    }
    virtual std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &, Scope *,
                                          std::vector<Scope *> &) {
        return nullptr;
    }
    virtual std::shared_ptr<Object> Eval(Scope *, std::vector<Scope *> &) {
        return nullptr;
    }
};

class Number : public Object {
public:
    explicit Number(int value) : Object(0), value_(value) {
    }
    int GetValue() {
        return value_;
    }
    virtual std::shared_ptr<Object> Eval(Scope *, std::vector<Scope *> &) final {
        return std::make_shared<Number>(value_);
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    explicit Symbol(std::string name) : Object(1), name_(name) {
    }
    std::string GetName() {
        return name_;
    }
    virtual std::shared_ptr<Object> Eval(Scope *, std::vector<Scope *> &);

private:
    std::string name_;
};

class Cell : public Object {
public:
    explicit Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : Object(2), first_(first), second_(second) {
    }
    std::shared_ptr<Object> GetFirst() {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() {
        return second_;
    }
    virtual std::shared_ptr<Object> Eval(Scope *scope, std::vector<Scope *> &);

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class Bool : public Object {
public:
    explicit Bool(bool det) : Object(3), det_(det) {
    }
    bool Get() {
        return det_;
    }
    virtual std::shared_ptr<Object> Eval(Scope *, std::vector<Scope *> &) final {
        return std::make_shared<Bool>(det_);
    }

private:
    bool det_;
};

bool IsNumber(const std::shared_ptr<Object> &obj);

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object> &obj);

bool IsCell(const std::shared_ptr<Object> &obj);

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object> &obj);

bool IsSymbol(const std::shared_ptr<Object> &obj);

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object> &obj);

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer);

std::shared_ptr<Object> Read(Tokenizer *tokenizer);

std::shared_ptr<Object> ReadVertex(Tokenizer *tokenizer);

bool IsBool(const std::shared_ptr<Object> &obj);

std::shared_ptr<Bool> AsBool(const std::shared_ptr<Object> &obj);

class RefFunction : public Object {
public:
    RefFunction(std::shared_ptr<Object> description, Scope *father, std::vector<Scope *> &scopes)
        : Object(5), description(description) {
        scope = new Scope(father);
        scopes.push_back(scope);
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *,
                                  std::vector<Scope *> &scopes) final {
        if (!IsCell(description)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(description);
        auto variables = cell->GetFirst();
        auto next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        auto condition = cell->GetFirst();
        auto cur_var = variables;
        auto cur_value = args;
        while (IsCell(cur_var)) {
            auto cell_var = AsCell(cur_var);
            auto var = cell_var->GetFirst();
            cur_var = cell_var->GetSecond();
            auto cell_val = AsCell(cur_value);
            auto value = cell_val->GetFirst()->Eval(scope, scopes);
            cur_value = cell_val->GetSecond();
            if (!IsSymbol(var)) {
                throw RuntimeError{};
            }
            scope->Init(AsSymbol(var)->GetName(), value);
        }
        auto result = condition->Eval(scope, scopes);
        auto cur = cell->GetSecond();
        while (IsCell(cur)) {
            cell = AsCell(cur);
            condition = cell->GetFirst();
            result = condition->Eval(scope, scopes);
            cur = cell->GetSecond();
        }
        return result;
    }
    std::shared_ptr<Object> description;
    Scope *scope;
};

class Function : public Object {
public:
    Function() : Object(4) {
    }
};

class ConsistFunction : public Object {
public:
    ConsistFunction() : Object(5) {
    }
};

bool IsFunction(const std::shared_ptr<Object> &obj);

class Quote : public ConsistFunction {
public:
    Quote() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *,
                                  std::vector<Scope *> &) final {
        if (!IsCell(args)) {
            throw RuntimeError{};
        }
        auto cell = AsCell(args);
        return cell->GetFirst();
    }
};

class List : public ConsistFunction {
public:
    List() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *,
                                  std::vector<Scope *> &) final {
        return args;
    }
};

class IsNumberFunction : public Function {
public:
    IsNumberFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        return std::make_shared<Bool>(IsNumber(args[0]));
    }
};

class IsBoolFunction : public Function {
public:
    IsBoolFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        return std::make_shared<Bool>(IsBool(args[0]));
    }
};

class IsSymbolFunction : public Function {
public:
    IsSymbolFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        return std::make_shared<Bool>(IsSymbol(args[0]));
    }
};

class IsPairFunction : public Function {
public:
    IsPairFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        return std::make_shared<Bool>(IsCell(args[0]));
    }
};

class IsListFunction : public Function {
public:
    IsListFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        if (args[0] == nullptr) {
            return std::make_shared<Bool>(true);
        }
        if (!IsCell(args[0])) {
            return std::make_shared<Bool>(false);
        }
        auto cur = args[0];
        while (IsCell(cur)) {
            cur = AsCell(cur)->GetSecond();
        }
        return std::make_shared<Bool>(cur == nullptr);
    }
};

class IsNullFunction : public Function {
public:
    IsNullFunction() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        return std::make_shared<Bool>(args[0] == nullptr);
    }
};

class Not : public Function {
public:
    Not() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1) {
            throw RuntimeError{};
        }
        auto arg = args[0];
        if (IsBool(arg)) {
            return std::make_shared<Bool>(!AsBool(arg)->Get());
        }
        return std::make_shared<Bool>(false);
    }
};

class And : public ConsistFunction {
public:
    And() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (next == nullptr) {
            return std::make_shared<Bool>(true);
        }
        auto cell = AsCell(next);
        auto arg = cell->GetFirst()->Eval(scope, scopes);
        if (IsBool(arg)) {
            if (!AsBool(arg)->Get()) {
                return std::make_shared<Bool>(false);
            }
        }
        while (next != nullptr && IsCell(next)) {
            cell = AsCell(next);
            arg = cell->GetFirst()->Eval(scope, scopes);
            if (IsBool(arg)) {
                if (!AsBool(arg)->Get()) {
                    return std::make_shared<Bool>(false);
                }
            }
            next = cell->GetSecond();
        }
        return arg;
    }
};

class Or : public ConsistFunction {
public:
    Or() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        while (next != nullptr && IsCell(next)) {
            auto cell = AsCell(next);
            auto arg = cell->GetFirst()->Eval(scope, scopes);
            if (IsBool(arg)) {
                if (AsBool(arg)->Get()) {
                    return std::make_shared<Bool>(true);
                }
            } else {
                return arg;
            }
            next = cell->GetSecond();
        }
        return std::make_shared<Bool>(false);
    }
};

class Sum : public Function {
public:
    Sum() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        int sum = 0;
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            sum += AsNumber(arg)->GetValue();
        }
        return std::make_shared<Number>(sum);
    }
};

class Minus : public Function {
public:
    Minus() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.empty() || !IsNumber(args[0])) {
            throw RuntimeError{};
        }
        int result = 2 * AsNumber(args[0])->GetValue();
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            result -= AsNumber(arg)->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class Prod : public Function {
public:
    Prod() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {

        int result = 1;
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            result *= AsNumber(arg)->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class Div : public Function {
public:
    Div() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.empty() || !IsNumber(args[0])) {
            throw RuntimeError{};
        }
        int result = AsNumber(args[0])->GetValue();
        result *= result;
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            result /= AsNumber(arg)->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class Min : public Function {
public:
    Min() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.empty() || !IsNumber(args[0])) {
            throw RuntimeError{};
        }
        int result = AsNumber(args[0])->GetValue();
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            result = std::min(result, AsNumber(arg)->GetValue());
        }
        return std::make_shared<Number>(result);
    }
};

class Max : public Function {
public:
    Max() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.empty() || !IsNumber(args[0])) {
            throw RuntimeError{};
        }
        int result = AsNumber(args[0])->GetValue();
        for (const auto &arg : args) {
            if (!IsNumber(arg)) {
                throw RuntimeError{};
            }
            result = std::max(result, AsNumber(arg)->GetValue());
        }
        return std::make_shared<Number>(result);
    }
};

class Abs : public Function {
public:
    Abs() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1 || !IsNumber(args[0])) {
            throw RuntimeError{};
        }
        int result = AsNumber(args[0])->GetValue();
        if (result < 0) {
            result = -result;
        }
        return std::make_shared<Number>(result);
    }
};

class Less : public Function {
public:
    Less() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            if (!IsNumber(args[i]) || !IsNumber(args[i + 1])) {
                throw RuntimeError{};
            }
            int left = AsNumber(args[i])->GetValue();
            int right = AsNumber(args[i + 1])->GetValue();
            if (left >= right) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class More : public Function {
public:
    More() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            if (!IsNumber(args[i]) || !IsNumber(args[i + 1])) {
                throw RuntimeError{};
            }
            int left = AsNumber(args[i])->GetValue();
            int right = AsNumber(args[i + 1])->GetValue();
            if (left <= right) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class LessEq : public Function {
public:
    LessEq() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            if (!IsNumber(args[i]) || !IsNumber(args[i + 1])) {
                throw RuntimeError{};
            }
            int left = AsNumber(args[i])->GetValue();
            int right = AsNumber(args[i + 1])->GetValue();
            if (left > right) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class MoreEq : public Function {
public:
    MoreEq() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            if (!IsNumber(args[i]) || !IsNumber(args[i + 1])) {
                throw RuntimeError{};
            }
            int left = AsNumber(args[i])->GetValue();
            int right = AsNumber(args[i + 1])->GetValue();
            if (left < right) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class Equal : public Function {
public:
    Equal() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        for (size_t i = 0; i + 1 < args.size(); ++i) {
            if (!IsNumber(args[i]) || !IsNumber(args[i + 1])) {
                throw RuntimeError{};
            }
            int left = AsNumber(args[i])->GetValue();
            int right = AsNumber(args[i + 1])->GetValue();
            if (left != right) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class Define : public ConsistFunction {
public:
    Define() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst();
        if (IsCell(first)) {
            next = cell->GetSecond();
            cell = AsCell(first);
            auto cur = cell->GetFirst();
            auto name = AsSymbol(cur)->GetName();
            auto var = cell->GetSecond();
            auto description = AsCell(next);
            auto result_cell = std::make_shared<Cell>(var, description);
            auto result = std::make_shared<RefFunction>(result_cell, scope, scopes);
            scope->Init(name, result);
            return nullptr;
        }
        if (!IsSymbol(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        scope->Init(AsSymbol(first)->GetName(), second);
        return nullptr;
    }
};

class Set : public ConsistFunction {
public:
    Set() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst();
        if (!IsSymbol(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        scope->Set(AsSymbol(first)->GetName(), second);
        return nullptr;
    }
};

class Cons : public Function {
public:
    Cons() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 2) {
            throw RuntimeError{};
        }
        return std::make_shared<Cell>(args[0], args[1]);
    }
};

class Car : public Function {
public:
    Car() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1 && IsCell(args[0])) {
            throw RuntimeError{};
        }
        auto cell = AsCell(args[0]);
        return cell->GetFirst();
    }
};

class Cdr : public Function {
public:
    Cdr() : Function() {
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>> &args) final {
        if (args.size() != 1 && IsCell(args[0])) {
            throw RuntimeError{};
        }
        auto cell = AsCell(args[0]);
        return cell->GetSecond();
    }
};

class SetCar : public ConsistFunction {
public:
    SetCar() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst();
        if (!IsSymbol(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        auto pair = scope->Get(AsSymbol(first)->GetName());
        if (!IsCell(pair)) {
            throw SyntaxError{};
        }
        auto right = AsCell(pair)->GetSecond();
        scope->Set(AsSymbol(first)->GetName(), std::make_shared<Cell>(second, right));
        return nullptr;
    }
};

class SetCdr : public ConsistFunction {
public:
    SetCdr() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst();
        if (!IsSymbol(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        auto pair = scope->Get(AsSymbol(first)->GetName());
        if (!IsCell(pair)) {
            throw SyntaxError{};
        }
        auto left = AsCell(pair)->GetFirst();
        scope->Set(AsSymbol(first)->GetName(), std::make_shared<Cell>(left, second));
        return nullptr;
    }
};

class ListRef : public ConsistFunction {
public:
    ListRef() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst()->Eval(scope, scopes);
        if (!IsCell(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        if (!IsNumber(second)) {
            throw SyntaxError{};
        }
        int index = AsNumber(second)->GetValue();
        auto list_it = first;
        while (index > 0) {
            auto local_cell = AsCell(list_it);
            auto local_next = local_cell->GetSecond();
            if (!IsCell(local_next)) {
                throw RuntimeError{};
            }
            list_it = local_next;
            index--;
        }
        return AsCell(list_it)->GetFirst();
    }
};

class ListTail : public ConsistFunction {
public:
    ListTail() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        auto next = args;
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(next);
        auto first = cell->GetFirst()->Eval(scope, scopes);
        if (!IsCell(first)) {
            throw SyntaxError{};
        }
        next = cell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        cell = AsCell(next);
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        auto second = cell->GetFirst()->Eval(scope, scopes);
        if (!IsNumber(second)) {
            throw SyntaxError{};
        }
        int index = AsNumber(second)->GetValue();
        auto list_it = first;
        while (index > 0) {
            if (!IsCell(list_it)) {
                throw RuntimeError{};
            }
            auto local_cell = AsCell(list_it);
            auto local_next = local_cell->GetSecond();
            list_it = local_next;
            index--;
        }
        return list_it;
    }
};

class If : public ConsistFunction {
public:
    If() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        if (!IsCell(args)) {
            throw SyntaxError{};
        }
        auto cell = AsCell(args);
        auto condition = cell->GetFirst()->Eval(scope, scopes);
        if (!IsBool(condition)) {
            throw SyntaxError{};
        }
        if (!IsCell(cell->GetSecond())) {
            throw SyntaxError{};
        }
        cell = AsCell(cell->GetSecond());
        auto first = cell->GetFirst();
        if (cell->GetSecond() == nullptr) {
            if (AsBool(condition)->Get()) {
                return first->Eval(scope, scopes);
            }
            return nullptr;
        }
        if (!IsCell(cell->GetSecond())) {
            throw SyntaxError{};
        }
        cell = AsCell(cell->GetSecond());
        auto second = cell->GetFirst();
        if (cell->GetSecond() != nullptr) {
            throw SyntaxError{};
        }
        if (AsBool(condition)->Get()) {
            return first->Eval(scope, scopes);
        } else {
            return second->Eval(scope, scopes);
        }
    }
};

class Lambda : public ConsistFunction {
public:
    Lambda() : ConsistFunction() {
    }
    std::shared_ptr<Object> Apply(const std::shared_ptr<Object> &args, Scope *scope,
                                  std::vector<Scope *> &scopes) final {
        if (!IsCell(args)) {
            throw SyntaxError{};
        }
        auto sell = AsCell(args);
        auto next = sell->GetSecond();
        if (!IsCell(next)) {
            throw SyntaxError{};
        }
        auto result = std::make_shared<RefFunction>(args, scope, scopes);
        result->Eval(scope, scopes);
        return result;
    }
};
