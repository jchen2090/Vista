#include "../src/parser/parser.h"
#include "../src/lexer/lexer.h"
#include "../src/token/token.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("Parse assignments", "[parser][assignments]") {
  SECTION("int x = 5") {
    Lexer lexer("int x = 5\n");
    auto root = Parser(lexer.tokenize()).parse();
    REQUIRE(root->statements.size() == 1);
    auto assign =
        dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(assign != nullptr);
    REQUIRE(assign->typeAnnotation == Type::INT);
    REQUIRE(assign->identifier == "x");
    auto expr = dynamic_cast<IntNode *>(assign->value.get());
    REQUIRE(expr != nullptr);
    REQUIRE(expr->value == 5);
  }

  SECTION("float pi = 3.14") {
    Lexer lexer("float pi = 3.14\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto assign =
        dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(assign->typeAnnotation == Type::FLOAT);
    REQUIRE(assign->identifier == "pi");
    auto expr = dynamic_cast<FloatNode *>(assign->value.get());
    REQUIRE(expr->value == 3.14);
  }

  SECTION("str msg = \"hello\"") {
    Lexer lexer("str msg = \"hello\"\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto assign =
        dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(assign->typeAnnotation == Type::STR);
    auto expr = dynamic_cast<StrNode *>(assign->value.get());
    REQUIRE(expr->value == "hello");
  }

  SECTION("identifier with numbers: int var123 = 42") {
    Lexer lexer("int var123 = 42\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto assign =
        dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(assign->identifier == "var123");
  }
}

TEST_CASE("Parse cout statements", "[parser][cout]") {
  SECTION("cout (42)") {
    Lexer lexer("cout (42)\n");
    auto root = Parser(lexer.tokenize()).parse();
    REQUIRE(root->statements.size() == 1);
    auto print = dynamic_cast<PrintStatement *>(root->statements[0].get());
    REQUIRE(print != nullptr);
    auto expr = dynamic_cast<IntNode *>(print->value.get());
    REQUIRE(expr->value == 42);
  }

  SECTION("cout (\"world\")") {
    Lexer lexer("cout (\"world\")\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto print = dynamic_cast<PrintStatement *>(root->statements[0].get());
    auto expr = dynamic_cast<StrNode *>(print->value.get());
    REQUIRE(expr->value == "world");
  }

  SECTION("cout (x) with identifier") {
    Lexer lexer("cout (x)\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto print = dynamic_cast<PrintStatement *>(root->statements[0].get());
    auto expr = dynamic_cast<IdentifierNode *>(print->value.get());
    REQUIRE(expr->identifier == "x");
  }

  SECTION("cout (3.14) with float") {
    Lexer lexer("cout (3.14)\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto print = dynamic_cast<PrintStatement *>(root->statements[0].get());
    auto expr = dynamic_cast<FloatNode *>(print->value.get());
    REQUIRE(expr->value == 3.14);
  }
}

TEST_CASE("Parse multiple statements", "[parser][multiple-statements]") {
  SECTION("multiple assignments") {
    Lexer lexer("int x = 5\nfloat y = 2.7\n");
    auto root = Parser(lexer.tokenize()).parse();
    REQUIRE(root->statements.size() == 2);
    auto first = dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(first->identifier == "x");
    auto second =
        dynamic_cast<AssignmentStatement *>(root->statements[1].get());
    REQUIRE(second->identifier == "y");
  }

  SECTION("multiple couts") {
    Lexer lexer("cout (1)\ncout (2)\n");
    auto root = Parser(lexer.tokenize()).parse();
    REQUIRE(root->statements.size() == 2);
    REQUIRE(dynamic_cast<PrintStatement *>(root->statements[0].get()) !=
            nullptr);
    REQUIRE(dynamic_cast<PrintStatement *>(root->statements[1].get()) !=
            nullptr);
  }

  SECTION("mixed assignments and couts") {
    Lexer lexer("int x = 5\ncout (x)\nfloat y = 3.14\n");
    auto root = Parser(lexer.tokenize()).parse();
    REQUIRE(root->statements.size() == 3);
    REQUIRE(dynamic_cast<AssignmentStatement *>(root->statements[0].get()) !=
            nullptr);
    REQUIRE(dynamic_cast<PrintStatement *>(root->statements[1].get()) !=
            nullptr);
    REQUIRE(dynamic_cast<AssignmentStatement *>(root->statements[2].get()) !=
            nullptr);
  }

  SECTION("cout after assignment with identifier check") {
    Lexer lexer("int a = 1\ncout (a)\n");
    auto root = Parser(lexer.tokenize()).parse();
    auto assign =
        dynamic_cast<AssignmentStatement *>(root->statements[0].get());
    REQUIRE(assign->identifier == "a");
    auto print = dynamic_cast<PrintStatement *>(root->statements[1].get());
    auto expr = dynamic_cast<IdentifierNode *>(print->value.get());
    REQUIRE(expr->identifier == "a");
  }
}

TEST_CASE("Reject invalid assignments", "[parser][bad]") {
  SECTION("missing identifier: int = 5") {
    Lexer lexer("int = 5\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("missing equals: int x 5") {
    Lexer lexer("int x 5\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("missing expression: int x =") {
    Lexer lexer("int x =\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("missing newline: int x = 5 (no newline)") {
    Lexer lexer("int x = 5");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }
}

TEST_CASE("Reject invalid cout statements", "[parser][bad]") {
  SECTION("missing open paren: cout 42)") {
    Lexer lexer("cout 42)\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("missing close paren: cout (42") {
    Lexer lexer("cout (42\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("missing newline: cout (42) (no newline)") {
    Lexer lexer("cout (42)");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }

  SECTION("empty expression: cout ()") {
    Lexer lexer("cout ()\n");
    REQUIRE_THROWS_AS(Parser(lexer.tokenize()).parse(), std::runtime_error);
  }
}
