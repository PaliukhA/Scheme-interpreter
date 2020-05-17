#pragma once

#include <iostream>
#include <variant>
#include <string>

struct QuoteToken {
    bool operator==(const QuoteToken &) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken &) const {
        return true;
    }
};

enum struct BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int64_t value;
    bool operator==(const ConstantToken &rhs) const {
        return value == rhs.value;
    }
};

struct SymbolToken {
    std::string name;
    bool operator==(const SymbolToken &rhs) const {
        return name == rhs.name;
    }
};

typedef std::variant<QuoteToken, DotToken, BracketToken, ConstantToken, SymbolToken> Token;

bool IsDigit(char x);

bool IsSpecialChar(char x);

bool IsOperation(char x);

bool IsCorrectChar(char x);

bool IsSpaces(char x);

class Tokenizer {
public:
    explicit Tokenizer(std::istream *in) : in_(in) {
        is_end_ = false;
        Next();
    }
    bool IsEnd() {
        return is_end_;
    }
    Token GetToken() {
        return cur_token_;
    }
    void Next() {
        RidOfSpace();
        char cur = in_->peek();
        if (cur == EOF) {
            is_end_ = true;
            return;
        }
        if (IsSpecialChar(cur)) {
            if (cur == '.') {
                cur_token_ = DotToken{};
            } else if (cur == '\'') {
                cur_token_ = QuoteToken{};
            } else if (cur == '(') {
                cur_token_ = BracketToken::OPEN;
            } else {
                cur_token_ = BracketToken::CLOSE;
            }
            in_->get();
        } else if (IsOperation(cur)) {
            in_->get();
            if (cur == '-' && IsDigit(in_->peek())) {
                cur_token_ = ConstantToken{-1 * ReadInt()};
            } else if (cur == '+' && IsDigit(in_->peek())) {
                cur_token_ = ConstantToken{ReadInt()};
            } else {
                cur_token_ = SymbolToken{std::string{cur}};
            }
        } else if (IsDigit(cur)) {
            cur_token_ = ConstantToken{ReadInt()};
        } else {
            cur_token_ = SymbolToken{ReadString()};
        }
    }

private:
    void RidOfSpace() {
        while (IsSpaces(in_->peek())) {
            in_->get();
        }
    }
    int64_t ReadInt() {  // without minus
        int64_t result = 0;
        while (IsDigit(in_->peek())) {
            result *= 10;
            result += in_->peek() - '0';
            in_->get();
        }
        return result;
    }
    std::string ReadString() {
        std::string result;
        while (IsCorrectChar(in_->peek())) {
            result += char(in_->peek());
            in_->get();
        }
        return result;
    }
    Token cur_token_;
    bool is_end_;
    std::istream *in_;
};