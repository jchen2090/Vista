#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/type/typeChecker.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <stdexcept>

namespace {
  std::unique_ptr<TypeChecker> makeChecker(const std::string &source) {
    Lexer lexer(source);
    auto root = Parser(lexer.tokenize()).parse();
    return std::make_unique<TypeChecker>(std::move(root));
  }
}

TEST_CASE("Type-check valid assignments", "[typechecker][valid]") {
  SECTION("int x = 5") {
    auto checker = makeChecker("int x = 5\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("float pi = 3.14") {
    auto checker = makeChecker("float pi = 3.14\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("str msg = \"hello\"") {
    auto checker = makeChecker("str msg = \"hello\"\n");
    REQUIRE_NOTHROW(checker->validate());
  }
}

TEST_CASE("Type-check valid variable references", "[typechecker][valid]") {
  SECTION("int y = x after int x = 5") {
    auto checker = makeChecker("int x = 5\nint y = x\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("float y = x after float x = 3.14") {
    auto checker = makeChecker("float x = 3.14\nfloat y = x\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("str y = x after str x = \"hi\"") {
    auto checker = makeChecker("str x = \"hi\"\nstr y = x\n");
    REQUIRE_NOTHROW(checker->validate());
  }
}

TEST_CASE("Type-check valid print statements", "[typechecker][valid]") {
  SECTION("cout (42)") {
    auto checker = makeChecker("cout (42)\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("cout (3.14)") {
    auto checker = makeChecker("cout (3.14)\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("cout (\"world\")") {
    auto checker = makeChecker("cout (\"world\")\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("cout (x) with declared variable") {
    auto checker = makeChecker("int x = 5\ncout (x)\n");
    REQUIRE_NOTHROW(checker->validate());
  }
}

TEST_CASE("Type-check multiple valid statements", "[typechecker][valid]") {
  SECTION("multiple assignments of different types") {
    auto checker =
        makeChecker("int x = 5\nfloat y = 3.14\nstr z = \"hi\"\n");
    REQUIRE_NOTHROW(checker->validate());
  }

  SECTION("mixed assignments and prints") {
    auto checker =
        makeChecker("int x = 5\ncout (x)\nfloat y = 3.14\ncout (y)\n");
    REQUIRE_NOTHROW(checker->validate());
  }
}

TEST_CASE("Type-check empty program", "[typechecker][valid]") {
  SECTION("root with no statements") {
    auto root = std::make_unique<RootNode>();
    TypeChecker checker(std::move(root));
    REQUIRE_NOTHROW(checker.validate());
  }
}

TEST_CASE("Reject assignment type mismatches", "[typechecker][bad]") {
  SECTION("int x = \"hello\"") {
    auto checker = makeChecker("int x = \"hello\"\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("int x = 3.14") {
    auto checker = makeChecker("int x = 3.14\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("float x = 5") {
    auto checker = makeChecker("float x = 5\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("float x = \"hello\"") {
    auto checker = makeChecker("float x = \"hello\"\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("str x = 42") {
    auto checker = makeChecker("str x = 42\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("str x = 3.14") {
    auto checker = makeChecker("str x = 3.14\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }
}

TEST_CASE("Reject undeclared variables", "[typechecker][bad]") {
  SECTION("int x = y where y is undeclared") {
    auto checker = makeChecker("int x = y\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("cout (x) where x is undeclared") {
    auto checker = makeChecker("cout (x)\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }
}

TEST_CASE("Reject mismatched variable reference types", "[typechecker][bad]") {
  SECTION("int y = x where x is float") {
    auto checker = makeChecker("float x = 3.14\nint y = x\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("float y = x where x is int") {
    auto checker = makeChecker("int x = 5\nfloat y = x\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("str y = x where x is int") {
    auto checker = makeChecker("int x = 5\nstr y = x\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }
}

TEST_CASE("First error stops validation", "[typechecker][bad]") {
  SECTION("error in second statement should still throw") {
    auto checker = makeChecker("int x = 5\nint y = \"hello\"\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }

  SECTION("multiple errors, first one is reported") {
    auto checker = makeChecker("int a = \"hi\"\nfloat b = \"there\"\n");
    REQUIRE_THROWS_AS(checker->validate(), std::runtime_error);
  }
}

TEST_CASE("Error message contains expected type names", "[typechecker][bad]") {
  SECTION("int x = \"hello\" error mentions str and int") {
    auto checker = makeChecker("int x = \"hello\"\n");
    REQUIRE_THROWS_WITH(
        checker->validate(),
        Catch::Matchers::ContainsSubstring(
            "TypeError: Cannot assign 'str' to variable 'x' of type int"));
  }

  SECTION("float x = 5 error mentions int and float") {
    auto checker = makeChecker("float x = 5\n");
    REQUIRE_THROWS_WITH(
        checker->validate(),
        Catch::Matchers::ContainsSubstring(
            "TypeError: Cannot assign 'int' to variable 'x' of type float"));
  }

  SECTION("undeclared variable error mentions unknown") {
    auto checker = makeChecker("int x = y\n");
    REQUIRE_THROWS_WITH(
        checker->validate(),
        Catch::Matchers::ContainsSubstring(
            "TypeError: Cannot assign 'unknown' to variable 'x' of type int"));
  }
}
