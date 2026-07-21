#include "../src/lexer/lexer.h"
#include "../src/token/token.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("Tokenize literals", "[lexer][literals]") {
  SECTION("integer: 42") {
    auto tokens = Lexer("42").tokenize();
    REQUIRE(tokens[0].type == TokenType::INT);
    REQUIRE(tokens[0].val == "42");
    REQUIRE(tokens[1].type == TokenType::EOF_TOKEN);
  }

  SECTION("integer starting with zero: 05") {
    auto tokens = Lexer("05").tokenize();
    REQUIRE(tokens[0].type == TokenType::INT);
    REQUIRE(tokens[0].val == "05");
  }

  SECTION("float: 3.14") {
    auto tokens = Lexer("3.14").tokenize();
    REQUIRE(tokens[0].type == TokenType::FLOAT);
    REQUIRE(tokens[0].val == "3.14");
  }

  SECTION("float ending with decimal: 5.") {
    auto tokens = Lexer("5.").tokenize();
    REQUIRE(tokens[0].type == TokenType::FLOAT);
    REQUIRE(tokens[0].val == "5.");
  }

  SECTION("string: \"hello world\"") {
    auto tokens = Lexer("\"hello world\"").tokenize();
    REQUIRE(tokens[0].type == TokenType::STR);
    REQUIRE(tokens[0].val == "hello world");
  }

  SECTION("empty string: \"\"") {
    auto tokens = Lexer("\"\"").tokenize();
    REQUIRE(tokens[0].type == TokenType::STR);
    REQUIRE(tokens[0].val == "");
  }
}

TEST_CASE("Tokenize keywords and identifiers", "[lexer][keywords][identifiers]") {
  SECTION("all keywords: int float str cout") {
    auto tokens = Lexer("int float str cout").tokenize();
    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == TokenType::INT_KEYWORD);
    REQUIRE(tokens[0].val == "int");
    REQUIRE(tokens[1].type == TokenType::FLOAT_KEYWORD);
    REQUIRE(tokens[1].val == "float");
    REQUIRE(tokens[2].type == TokenType::STR_KEYWORD);
    REQUIRE(tokens[2].val == "str");
    REQUIRE(tokens[3].type == TokenType::COUT_KEYWORD);
    REQUIRE(tokens[3].val == "cout");
  }

  SECTION("identifiers with numbers: var1 foo123 bar") {
    auto tokens = Lexer("var1 foo123 bar").tokenize();
    REQUIRE(tokens.size() == 4);
    for (size_t i = 0; i < 3; i++) {
      REQUIRE(tokens[i].type == TokenType::IDENTIFIER);
      REQUIRE(!tokens[i].val.empty());
    }
  }

  SECTION("assignment: int x = 5") {
    auto tokens = Lexer("int x = 5").tokenize();
    REQUIRE(tokens[0].type == TokenType::INT_KEYWORD);
    REQUIRE(tokens[0].val == "int");
    REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].val == "x");
    REQUIRE(tokens[2].type == TokenType::ASSIGN);
    REQUIRE(tokens[2].val == "=");
    REQUIRE(tokens[3].type == TokenType::INT);
    REQUIRE(tokens[3].val == "5");
  }

  SECTION("cout with parentheses: cout (42)") {
    auto tokens = Lexer("cout (42)").tokenize();
    REQUIRE(tokens[0].type == TokenType::COUT_KEYWORD);
    REQUIRE(tokens[1].type == TokenType::OPEN_PARENTHESES);
    REQUIRE(tokens[2].type == TokenType::INT);
    REQUIRE(tokens[3].type == TokenType::CLOSE_PARENTHESES);
  }
}

TEST_CASE("Tokenize parentheses", "[lexer][brackets]") {
  SECTION("standalone: ()") {
    auto tokens = Lexer("()").tokenize();
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::OPEN_PARENTHESES);
    REQUIRE(tokens[1].type == TokenType::CLOSE_PARENTHESES);
  }

  SECTION("with identifier: (a)") {
    auto tokens = Lexer("(a)").tokenize();
    REQUIRE(tokens[0].type == TokenType::OPEN_PARENTHESES);
    REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].val == "a");
    REQUIRE(tokens[2].type == TokenType::CLOSE_PARENTHESES);
  }

  SECTION("function-like: foo(bar)") {
    auto tokens = Lexer("foo(bar)").tokenize();
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[0].val == "foo");
    REQUIRE(tokens[1].type == TokenType::OPEN_PARENTHESES);
    REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[2].val == "bar");
    REQUIRE(tokens[3].type == TokenType::CLOSE_PARENTHESES);
  }
}

TEST_CASE("Tokenize numbers", "[lexer][numbers]") {
  SECTION("multiple floats: 1.5 2.7") {
    auto tokens = Lexer("1.5 2.7").tokenize();
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::FLOAT);
    REQUIRE(tokens[0].val == "1.5");
    REQUIRE(tokens[1].type == TokenType::FLOAT);
    REQUIRE(tokens[1].val == "2.7");
  }

  SECTION("mixed types: 1 2 3.14") {
    auto tokens = Lexer("1 2 3.14").tokenize();
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::INT);
    REQUIRE(tokens[0].val == "1");
    REQUIRE(tokens[1].type == TokenType::INT);
    REQUIRE(tokens[1].val == "2");
    REQUIRE(tokens[2].type == TokenType::FLOAT);
    REQUIRE(tokens[2].val == "3.14");
  }
}

TEST_CASE("Tokenize newlines and whitespace", "[lexer][edge-cases]") {
  SECTION("newline between tokens: a\\nb") {
    auto tokens = Lexer("a\nb").tokenize();
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].type == TokenType::EOL_TOKEN);
    REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[2].val == "b");
    REQUIRE(tokens[3].type == TokenType::EOF_TOKEN);
  }

  SECTION("multiple newlines: \\n\\n\\n") {
    auto tokens = Lexer("\n\n\n").tokenize();
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == TokenType::EOL_TOKEN);
    REQUIRE(tokens[1].type == TokenType::EOL_TOKEN);
    REQUIRE(tokens[2].type == TokenType::EOL_TOKEN);
  }

  SECTION("trailing newline: hello\\n") {
    auto tokens = Lexer("hello\n").tokenize();
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].type == TokenType::EOL_TOKEN);
  }

  SECTION("empty input") {
    auto tokens = Lexer("").tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
    REQUIRE(tokens[0].val == "");
  }

  SECTION("whitespace only") {
    auto tokens = Lexer("    ").tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
  }

  SECTION("whitespace with newlines") {
    auto tokens = Lexer("  \n  \n").tokenize();
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::EOL_TOKEN);
    REQUIRE(tokens[1].type == TokenType::EOL_TOKEN);
    REQUIRE(tokens[2].type == TokenType::EOF_TOKEN);
  }
}
