#pragma once

#include <exception>
#include <string>

struct SyntaxError : public std::exception {};

class RuntimeError : public std::exception {};

class NameError : public std::exception {};
