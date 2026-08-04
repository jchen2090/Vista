#include "../src/type/typeChecker.h"
#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <stdexcept>

namespace {
std::unique_ptr<RootNode> makeChecker(const std::string &source) {
  Lexer lexer(source);
  auto root = Parser(lexer.tokenize()).parse();
  return root;
}
} // namespace

TEST_CASE("Type-check valid assignments", "[typechecker][valid]") {
  TypeChecker tc;
  SECTION("int x = 5") {
    auto root = makeChecker("int x = 5\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("float pi = 3.14") {
    auto root = makeChecker("float pi = 3.14\n");
    REQUIRE_NOTHROW(tc.validate((*root)));
  }

  SECTION("str msg = \"hello\"") {
    auto root = makeChecker("str msg = \"hello\"\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }
}

TEST_CASE("Type-check valid variable references", "[typechecker][valid]") {
  TypeChecker tc;
  SECTION("int y = x after int x = 5") {
    auto root = makeChecker("int x = 5\nint y = x\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("float y = x after float x = 3.14") {
    auto root = makeChecker("float x = 3.14\nfloat y = x\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("str y = x after str x = \"hi\"") {
    auto root = makeChecker("str x = \"hi\"\nstr y = x\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }
}

TEST_CASE("Type-check valid print statements", "[typechecker][valid]") {
  TypeChecker tc;
  SECTION("cout (42)") {
    auto root = makeChecker("cout (42)\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("cout (3.14)") {
    auto root = makeChecker("cout (3.14)\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("cout (\"world\")") {
    auto root = makeChecker("cout (\"world\")\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("cout (x) with declared variable") {
    auto root = makeChecker("int x = 5\ncout (x)\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }
}

TEST_CASE("Type-check multiple valid statements", "[typechecker][valid]") {
  TypeChecker tc;
  SECTION("multiple assignments of different types") {
    auto root = makeChecker("int x = 5\nfloat y = 3.14\nstr z = \"hi\"\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }

  SECTION("mixed assignments and prints") {
    auto root = makeChecker("int x = 5\ncout (x)\nfloat y = 3.14\ncout (y)\n");
    REQUIRE_NOTHROW(tc.validate(*root));
  }
}

TEST_CASE("Type-check empty program", "[typechecker][valid]") {
  TypeChecker tc;
  SECTION("root with no statements") {
    auto root = std::make_unique<RootNode>();
    REQUIRE_NOTHROW(tc.validate(*root));
  }
}

TEST_CASE("Reject assignment type mismatches", "[typechecker][bad]") {
  TypeChecker tc;
  SECTION("int x = \"hello\"") {
    auto root = makeChecker("int x = \"hello\"\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("int x = 3.14") {
    auto root = makeChecker("int x = 3.14\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("float x = 5") {
    auto root = makeChecker("float x = 5\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("float x = \"hello\"") {
    auto root = makeChecker("float x = \"hello\"\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("str x = 42") {
    auto root = makeChecker("str x = 42\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("str x = 3.14") {
    auto root = makeChecker("str x = 3.14\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }
}

TEST_CASE("Reject undeclared variables", "[typechecker][bad]") {
  TypeChecker tc;
  SECTION("int x = y where y is undeclared") {
    auto root = makeChecker("int x = y\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("cout (x) where x is undeclared") {
    auto root = makeChecker("cout (x)\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }
}

TEST_CASE("Reject mismatched variable reference types", "[typechecker][bad]") {
  TypeChecker tc;
  SECTION("int y = x where x is float") {
    auto root = makeChecker("float x = 3.14\nint y = x\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("float y = x where x is int") {
    auto root = makeChecker("int x = 5\nfloat y = x\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("str y = x where x is int") {
    auto root = makeChecker("int x = 5\nstr y = x\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }
}

TEST_CASE("First error stops validation", "[typechecker][bad]") {
  TypeChecker tc;
  SECTION("error in second statement should still throw") {
    auto root = makeChecker("int x = 5\nint y = \"hello\"\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }

  SECTION("multiple errors, first one is reported") {
    auto root = makeChecker("int a = \"hi\"\nfloat b = \"there\"\n");
    REQUIRE_THROWS_AS(tc.validate(*root), std::runtime_error);
  }
}

TEST_CASE("Error message contains expected type names", "[typechecker][bad]") {
  TypeChecker tc;
  SECTION("int x = \"hello\" error mentions str and int") {
    auto root = makeChecker("int x = \"hello\"\n");
    REQUIRE_THROWS_WITH(
        tc.validate(*root),
        Catch::Matchers::ContainsSubstring(
            "TypeError: Cannot assign 'str' to variable 'x' of type int"));
  }

  SECTION("float x = 5 error mentions int and float") {
    auto root = makeChecker("float x = 5\n");
    REQUIRE_THROWS_WITH(
        tc.validate(*root),
        Catch::Matchers::ContainsSubstring(
            "TypeError: Cannot assign 'int' to variable 'x' of type float"));
  }
}
