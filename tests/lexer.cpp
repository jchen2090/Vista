#include "../src/lexer/lexer.h"
#include "../src/token/token.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("Tokenize Assignment", "[lexer][operators]") {
  std::string code = "int x = 5";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() >= 4);

  SECTION("First token is INT_KEYWORD") {
    REQUIRE(tokens[0].type == TokenType::INT_KEYWORD);
    REQUIRE(tokens[0].val == "int");
  }

  SECTION("Seconed token is IDENTIFIER") {
    REQUIRE(tokens[1].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].val == "x");
  }

  SECTION("Third token is ASSIGNMENT") {
    REQUIRE(tokens[2].type == TokenType::ASSIGN);
    REQUIRE(tokens[2].val == "=");
  }

  SECTION("Fourth token is INT") {
    REQUIRE(tokens[3].type == TokenType::INT);
    REQUIRE(tokens[3].val == "5");
  }
}

TEST_CASE("Tokenize string literal", "[lexer][strings]") {
  std::string code = "\"hello world\"";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() >= 2);

  SECTION("First token is STR") {
    REQUIRE(tokens[0].type == TokenType::STR);
    REQUIRE(tokens[0].val == "hello world");
  }

  SECTION("Second token is EOF") {
    REQUIRE(tokens[1].type == TokenType::EOF_TOKEN);
    REQUIRE(tokens[1].val == "\0");
  }
}

TEST_CASE("Tokenize integer literal", "[lexer][numbers]") {
  std::string code = "42";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 2);

  SECTION("First token is INT") {
    REQUIRE(tokens[0].type == TokenType::INT);
    REQUIRE(tokens[0].val == "42");
  }
}

TEST_CASE("Tokenize float literal", "[lexer][numbers]") {
  std::string code = "3.14";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 2);

  SECTION("First token is FLOAT") {
    REQUIRE(tokens[0].type == TokenType::FLOAT);
    REQUIRE(tokens[0].val == "3.14");
  }
}

TEST_CASE("Tokenize multiple float literals", "[lexer][numbers]") {
  std::string code = "1.5 2.7";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 3); // 1.5, space, 2.7, EOF

  SECTION("Float literals are correct") {
    REQUIRE(tokens[0].type == TokenType::FLOAT);
    REQUIRE(tokens[0].val == "1.5");
    REQUIRE(tokens[1].type == TokenType::FLOAT);
    REQUIRE(tokens[1].val == "2.7");
  }
}

TEST_CASE("Tokenize multiple number types", "[lexer][numbers]") {
  std::string code = "1 2 3.14";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4); // 1, 2, 3.14, EOF

  SECTION("Mixed number types") {
    REQUIRE(tokens[0].type == TokenType::INT);
    REQUIRE(tokens[0].val == "1");
    REQUIRE(tokens[1].type == TokenType::INT);
    REQUIRE(tokens[1].val == "2");
    REQUIRE(tokens[2].type == TokenType::FLOAT);
    REQUIRE(tokens[2].val == "3.14");
  }
}

TEST_CASE("Tokenize keywords", "[lexer][keywords]") {
  std::string code = "int float str cout";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 5); // int, float, str, cout, EOF

  SECTION("INT_KEYWORD token") {
    REQUIRE(tokens[0].type == TokenType::INT_KEYWORD);
    REQUIRE(tokens[0].val == "int");
  }

  SECTION("FLOAT_KEYWORD token") {
    REQUIRE(tokens[1].type == TokenType::FLOAT_KEYWORD);
    REQUIRE(tokens[1].val == "float");
  }

  SECTION("STR_KEYWORD token") {
    REQUIRE(tokens[2].type == TokenType::STR_KEYWORD);
    REQUIRE(tokens[2].val == "str");
  }

  SECTION("COUT_KEYWORD token") {
    REQUIRE(tokens[3].type == TokenType::COUT_KEYWORD);
    REQUIRE(tokens[3].val == "cout");
  }
}

TEST_CASE("Tokenize parentheses", "[lexer][brackets]") {
  std::string code = "(a)";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  REQUIRE(tokens.size() == 4); // (, a, ), EOF

  SECTION("OPEN_PARENTHESES for both parens") {
    REQUIRE(tokens[0].type == TokenType::OPEN_PARENTHESES);
    REQUIRE(tokens[0].val == "(");
    REQUIRE(tokens[2].type == TokenType::CLOSE_PARENTHESES);
    REQUIRE(tokens[2].val == ")");
  }
}

TEST_CASE("Tokenize empty string", "[lexer][edge-cases]") {
  std::string code = "";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  SECTION("Empty input produces single EOF_TOKEN") {
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
    REQUIRE(tokens[0].val == "");
  }
}

TEST_CASE("Tokenize whitespace only", "[lexer][edge-cases]") {
  std::string code = "    ";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  SECTION("Whitespace-only input produces single EOF_TOKEN") {
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
    REQUIRE(tokens[0].val == "");
  }
}

TEST_CASE("Tokenize identifiers with numbers", "[lexer][identifiers]") {
  std::string code = "var1 foo123 bar";
  Lexer lexer(code);
  std::vector<Token> tokens = lexer.tokenize();

  SECTION("All are IDENTIFIER tokens") {
    for (size_t i = 0; i < 3; i++) {
      REQUIRE(tokens[i].type == TokenType::IDENTIFIER);
      REQUIRE(!tokens[i].val.empty());
    }
  }
}
