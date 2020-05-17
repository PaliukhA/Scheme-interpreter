#pragma once

#include <catch.hpp>
#include "scheme.h"

struct SchemeTest {
    SchemeTest() : compilation() {
    }

    void ExpectEq(std::string expression, std::string result) {
        std::string my_result = compilation.Build(expression);
        REQUIRE(my_result == result);
    }

    void ExpectNoError(std::string expression) {
        compilation.Build(expression);
    }

    void ExpectSyntaxError(std::string expression) {
        REQUIRE_THROWS_AS(compilation.Build(expression), SyntaxError);
    }

    void ExpectRuntimeError(std::string expression) {
        REQUIRE_THROWS_AS(compilation.Build(expression), RuntimeError);
    }

    void ExpectNameError(std::string expression) {
        REQUIRE_THROWS_AS(compilation.Build(expression), NameError);
    }
    Compilation compilation;
};
