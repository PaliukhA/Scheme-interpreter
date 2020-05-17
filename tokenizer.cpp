#include "tokenizer.h"

bool IsDigit(char x) {
    return isdigit(x);
}

bool IsSpecialChar(char x) {
    return x == '(' || x == ')' || x == '\'' || x == '.';
}

bool IsOperation(char x) {
    return x == '+' || x == '-' || x == '*';
}

bool IsSpaces(char x) {
    return x == ' ' || x == '\n' || x == 32;
}

bool IsCorrectChar(char x) {
    return !IsSpecialChar(x) && x != EOF && !IsSpaces(x);
}