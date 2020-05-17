#include "scheme_test.h"

TEST_CASE_METHOD(SchemeTest, "1SimpleLambda") {
    ExpectEq("(+ 2 4)", "6");
}

TEST_CASE_METHOD(SchemeTest, "1LambdaBodyHasImplicitBegin") {
    ExpectEq("(* 3 4)", "12");
}

TEST_CASE_METHOD(SchemeTest, "1SlowSum") {
    ExpectNoError("(define slow-add (lambda (x y) (if (= x 0) y (slow-add (- x 1) (+ y 1)))))");
    ExpectEq("(slow-add 3 3)", "6");
    ExpectEq("(slow-add 100 100)", "200");
}

TEST_CASE_METHOD(SchemeTest, "1LambdaSyntax") {
    ExpectSyntaxError("(lambda)");
    ExpectSyntaxError("(lambda x)");
    ExpectSyntaxError("(lambda (x))");
}

TEST_CASE_METHOD(SchemeTest, "1DefineLambdaSugar") {
    ExpectNoError("(define (inc x) (+ x 1))");
    ExpectEq("(inc -1)", "0");

    ExpectNoError("(define (add x y) (+ x y 1))");
    ExpectEq("(add -10 10)", "1");

    ExpectNoError("(define (zero) 0)");
    ExpectEq("(zero)", "0");
}