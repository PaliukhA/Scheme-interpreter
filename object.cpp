#include "object.h"

bool IsNumber(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return false;
    }
    return obj->type_ == 0;
}
std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object> &obj) {
    Number *number = dynamic_cast<Number *>(obj.get());
    return std::make_shared<Number>(*number);
}

bool IsCell(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return false;
    }
    return obj->type_ == 2;
}
std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object> &obj) {
    Cell *cell = dynamic_cast<Cell *>(obj.get());
    return std::make_shared<Cell>(*cell);
}

bool IsSymbol(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return 0;
    }
    return obj->type_ == 1;
}

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object> &obj) {
    Symbol *symbol = dynamic_cast<Symbol *>(obj.get());
    return std::make_shared<Symbol>(*symbol);
}

std::shared_ptr<Object> ReadVertex(Tokenizer *tokenizer) {
    if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        auto result = ReadList(tokenizer);
        if (result != nullptr) {
            if (!IsCell(result)) {
                throw SyntaxError{};
            }
            auto cell = AsCell(result);
            if (cell->GetFirst() == nullptr) {
                throw SyntaxError{};
            }
        }
        return result;
    }
    if (tokenizer->GetToken() == Token{QuoteToken{}}) {
        tokenizer->Next();
        Cell result(std::make_shared<Symbol>(Symbol("quote")),
                    std::make_shared<Cell>(ReadVertex(tokenizer), nullptr));
        return std::make_shared<Cell>(result);
    }
    Token token = tokenizer->GetToken();
    tokenizer->Next();
    if (auto symbol_token_ref = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(symbol_token_ref->name);
    }
    if (auto constant_token_ref = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(constant_token_ref->value);
    }
    return nullptr;
}

std::shared_ptr<Object> Read(Tokenizer *tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        throw SyntaxError{};
    }
    auto ret = ReadVertex(tokenizer);
    if (!tokenizer->IsEnd()) {
        throw SyntaxError{};
    }
    if (ret == nullptr) {
        throw RuntimeError{};
    }
    return ret;
}

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError{};
    }
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    }
    if (tokenizer->GetToken() == Token{DotToken{}}) {
        tokenizer->Next();
        auto second = ReadVertex(tokenizer);
        if (tokenizer->IsEnd() || !(tokenizer->GetToken() == Token{BracketToken::CLOSE})) {
            throw SyntaxError{};
        }
        tokenizer->Next();
        return second;
    }
    auto left = ReadVertex(tokenizer);
    auto right = ReadList(tokenizer);
    return std::make_shared<Cell>(left, right);
}

bool IsBool(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return false;
    }
    return obj->type_ == 3;
}

std::shared_ptr<Bool> AsBool(const std::shared_ptr<Object> &obj) {
    Bool *det = dynamic_cast<Bool *>(obj.get());
    return std::make_shared<Bool>(*det);
}

bool IsFunction(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return false;
    }
    return obj->type_ == 4 || obj->type_ == 5;
}
bool IsConsistFunction(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return false;
    }
    return obj->type_ == 5;
}

std::shared_ptr<Function> AsFunction(const std::shared_ptr<Object> &obj) {
    Function *func = dynamic_cast<Function *>(obj.get());
    return std::make_shared<Function>(*func);
}

std::shared_ptr<Object> Cell::Eval(Scope *scope, std::vector<Scope *> &scopes) {
    auto func = first_->Eval(scope, scopes);
    if (!IsFunction(func)) {
        throw RuntimeError{};
    }
    if (!IsConsistFunction(func)) {
        std::vector<std::shared_ptr<Object>> args;
        auto next = second_;
        while (next != nullptr && IsCell(next)) {
            auto cell = AsCell(next);
            auto arg = cell->first_->Eval(scope, scopes);
            args.push_back(arg);
            next = cell->second_;
        }
        return func->Apply(args);
    } else {
        return func->Apply(second_, scope, scopes);
    }
}

std::shared_ptr<Object> Symbol::Eval(Scope *scope, std::vector<Scope *> &) {
    if (name_ == "#t") {
        return std::make_shared<Bool>(true);
    }
    if (name_ == "#f") {
        return std::make_shared<Bool>(false);
    }
    if (name_ == "quote") {
        return std::make_shared<Quote>();
    }
    if (name_ == "list") {
        return std::make_shared<List>();
    }
    if (name_ == "list-ref") {
        return std::make_shared<ListRef>();
    }
    if (name_ == "list-tail") {
        return std::make_shared<ListTail>();
    }
    if (name_ == "number?") {
        return std::make_shared<IsNumberFunction>();
    }
    if (name_ == "boolean?") {
        return std::make_shared<IsBoolFunction>();
    }
    if (name_ == "symbol?") {
        return std::make_shared<IsSymbolFunction>();
    }
    if (name_ == "pair?") {
        return std::make_shared<IsPairFunction>();
    }
    if (name_ == "list?") {
        return std::make_shared<IsListFunction>();
    }
    if (name_ == "null?") {
        return std::make_shared<IsNullFunction>();
    }
    if (name_ == "not") {
        return std::make_shared<Not>();
    }
    if (name_ == "and") {
        return std::make_shared<And>();
    }
    if (name_ == "or") {
        return std::make_shared<Or>();
    }
    if (name_ == "+") {
        return std::make_shared<Sum>();
    }
    if (name_ == "-") {
        return std::make_shared<Minus>();
    }
    if (name_ == "*") {
        return std::make_shared<Prod>();
    }
    if (name_ == "/") {
        return std::make_shared<Div>();
    }
    if (name_ == "min") {
        return std::make_shared<Min>();
    }
    if (name_ == "max") {
        return std::make_shared<Max>();
    }
    if (name_ == "abs") {
        return std::make_shared<Abs>();
    }
    if (name_ == "<") {
        return std::make_shared<Less>();
    }
    if (name_ == ">") {
        return std::make_shared<More>();
    }
    if (name_ == "<=") {
        return std::make_shared<LessEq>();
    }
    if (name_ == ">=") {
        return std::make_shared<MoreEq>();
    }
    if (name_ == "=") {
        return std::make_shared<Equal>();
    }
    if (name_ == "cons") {
        return std::make_shared<Cons>();
    }
    if (name_ == "car") {
        return std::make_shared<Car>();
    }
    if (name_ == "cdr") {
        return std::make_shared<Cdr>();
    }
    if (name_ == "set-car!") {
        return std::make_shared<SetCar>();
    }
    if (name_ == "set-cdr!") {
        return std::make_shared<SetCdr>();
    }
    if (name_ == "define") {
        return std::make_shared<Define>();
    }
    if (name_ == "set!") {
        return std::make_shared<Set>();
    }
    if (name_ == "if") {
        return std::make_shared<If>();
    }
    if (name_ == "lambda") {
        return std::make_shared<Lambda>();
    }
    return scope->Get(name_);
}
