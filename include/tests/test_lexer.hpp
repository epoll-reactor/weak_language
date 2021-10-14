#ifndef WEAK_TESTS_LEXER_HPP
#define WEAK_TESTS_LEXER_HPP

#include "../lexer/lexer.hpp"
#include "../tests/test_utility.hpp"

#include <iomanip>
#include <random>
#include <sstream>

namespace lexer_detail {

void run_test(std::string_view data, std::vector<Token> assertion_tokens) {
  Lexer lexer(std::istringstream{data.data()});
  const std::vector<Token> tokens = lexer.tokenize();
  assertion_tokens.emplace_back(Token{"", token_t::END_OF_DATA});
  assert(tokens.size() == assertion_tokens.size());
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i].type != assertion_tokens[i].type) {
      throw LexicalError(dispatch_token(tokens[i].type) + " got, but " + dispatch_token(assertion_tokens[i].type) + " required");
    }
    if (tokens[i].data != assertion_tokens[i].data) {
      throw LexicalError(tokens[i].data + " got, but " + assertion_tokens[i].data + " required");
    }
  }
}

void assert_exception(std::string_view data) {
  trace_error(data, [&data] {
    Lexer lexer(std::istringstream{data.data()});
    lexer.tokenize();

    /// Will be skipped if exception thrown from analyzer function
    assert(false && "Error expected");
  });
}

}// namespace lexer_detail

// clang-format off
void lexer_number_literal_tests() {
    lexer_detail::run_test("2 2", { Token{"2", token_t::NUM}, Token{"2", token_t::NUM} });
    lexer_detail::run_test("2 2 ", { Token{"2", token_t::NUM}, Token{"2", token_t::NUM} });
    lexer_detail::run_test(" 2 2", { Token{"2", token_t::NUM}, Token{"2", token_t::NUM} });
    lexer_detail::run_test("        2       2       ", { Token{"2", token_t::NUM}, Token{"2", token_t::NUM} });
    lexer_detail::run_test("111.111", {  Token{"111.111", token_t::FLOAT}, });
    lexer_detail::run_test("111.111 222.222", { Token{"111.111", token_t::FLOAT}, Token{"222.222", token_t::FLOAT} });
    lexer_detail::run_test(" 111.111 222.222 333.333 444.444 555.555 ", {
        Token{"111.111", token_t::FLOAT},
        Token{"222.222", token_t::FLOAT},
        Token{"333.333", token_t::FLOAT},
        Token{"444.444", token_t::FLOAT},
        Token{"555.555", token_t::FLOAT},
    });

    lexer_detail::assert_exception("1.");
    lexer_detail::assert_exception("1........");
    lexer_detail::assert_exception("1..2");
}

void lexer_string_literal_tests() {
  lexer_detail::run_test("\"\"", {Token{"", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"text\"", {Token{"text", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"111.222\"", {Token{"111.222", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"text \\\" with escaped character \"", {Token{"text \" with escaped character ", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"Текст на русском языке\"", {Token{"Текст на русском языке", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"Türkçe metin\"", {Token{"Türkçe metin", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f\"", {Token{"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\" \\\"\"", {Token{" \"", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\"\\\"", {Token{"\\", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\" \\\\\"", {Token{" \\", token_t::STRING_LITERAL}});
  lexer_detail::run_test("\" \"", {Token{" ", token_t::STRING_LITERAL}});
  lexer_detail::run_test(" \"?\\\"\\\"\" \"?\\\"\\\"\\\"\" ", {Token{"?\"\"", token_t::STRING_LITERAL}, Token{"?\"\"\"", token_t::STRING_LITERAL}});

  lexer_detail::assert_exception("\"text without closing quote");
//  lexer_detail::assert_exception("\"\\");
}

void lexer_symbol_tests() {
  lexer_detail::run_test("Symbol", {Token{"Symbol", token_t::SYMBOL}});
  lexer_detail::run_test("A B C", {Token{"A", token_t::SYMBOL}, Token{"B", token_t::SYMBOL}, Token{"C", token_t::SYMBOL}});
  lexer_detail::run_test("a1b2c3d4 a000000a", {Token{"a1b2c3d4", token_t::SYMBOL}, Token{"a000000a", token_t::SYMBOL}});
  lexer_detail::run_test("     a1b2c3d4      a000000a       ", {Token{"a1b2c3d4", token_t::SYMBOL}, Token{"a000000a", token_t::SYMBOL}});
  lexer_detail::run_test("test?", {Token{"test?", token_t::SYMBOL}});

  lexer_detail::assert_exception("1A");
  lexer_detail::assert_exception("0_0");
}

void lexer_operator_tests() {
  lexer_detail::run_test("((((((((((", std::vector<Token>(10, Token{"", token_t::LEFT_PAREN}));
  lexer_detail::run_test("))))))))))", std::vector<Token>(10, Token{"", token_t::RIGHT_PAREN}));

  std::string increments(200, '+');
  lexer_detail::run_test(increments, std::vector<Token>(100, Token{"", token_t::INC}));

  std::string decrements(200, '-');
  lexer_detail::run_test(decrements, std::vector<Token>(100, Token{"", token_t::DEC}));

  lexer_detail::run_test("!", { Token{"", token_t::NEGATION} });
  lexer_detail::run_test("==", { Token{"", token_t::EQ} });
  lexer_detail::run_test("!=", { Token{"", token_t::NEQ} });
  lexer_detail::run_test("+", { Token{"", token_t::PLUS} });
  lexer_detail::run_test("++", { Token{"", token_t::INC} });
  lexer_detail::run_test("+++", {Token{"", token_t::INC}, Token{"", token_t::PLUS}});
  lexer_detail::run_test("++++", { Token{"", token_t::INC}, Token{"", token_t::INC}, });
  lexer_detail::run_test("+++++", {Token{"", token_t::INC}, Token{"", token_t::INC}, Token{"", token_t::PLUS}});
  lexer_detail::run_test("++ ++ +", {Token{"", token_t::INC}, Token{"", token_t::INC}, Token{"", token_t::PLUS}});
  lexer_detail::run_test("+ ++ + +", {Token{"", token_t::PLUS}, Token{"", token_t::INC}, Token{"", token_t::PLUS}, Token{"", token_t::PLUS}});
  lexer_detail::run_test("+ += /=", {Token{"", token_t::PLUS}, Token{"", token_t::PLUS_ASSIGN}, Token{"", token_t::SLASH_ASSIGN}});
  lexer_detail::run_test("++=/=", {Token{"", token_t::INC}, Token{"", token_t::ASSIGN}, Token{"", token_t::SLASH_ASSIGN}});
  lexer_detail::run_test("...,,,", {Token{"", token_t::DOT}, Token{"", token_t::DOT}, Token{"", token_t::DOT}, Token{"", token_t::COMMA}, Token{"", token_t::COMMA}, Token{"", token_t::COMMA}});
  lexer_detail::run_test("++--++--+-+-++--+++---+", {
    Token{"", token_t::INC},
    Token{"", token_t::DEC},
    Token{"", token_t::INC},
    Token{"", token_t::DEC},
    Token{"", token_t::PLUS},
    Token{"", token_t::MINUS},
    Token{"", token_t::PLUS},
    Token{"", token_t::MINUS},
    Token{"", token_t::INC},
    Token{"", token_t::DEC},
    Token{"", token_t::INC},
    Token{"", token_t::PLUS},
    Token{"", token_t::DEC},
    Token{"", token_t::MINUS},
    Token{"", token_t::PLUS},
  });
  lexer_detail::run_test("\0", { /* None */ });
  // Unknown operators
  lexer_detail::assert_exception("+++`+++");
  lexer_detail::assert_exception("@@@@@@@");
}
// clang-format on

void lexer_expression_tests() {
  lexer_detail::run_test(
      "void f(int a, int b, int c) {"
      "  if (true) {"
      "    int variable-0 = 123;"
      "  } else {"
      "    string literal-1 = \"Lorem ipsum\";"
      "  }"
      "}",
      {Token{"void", token_t::SYMBOL},
       Token{"f", token_t::SYMBOL},
       Token{"", token_t::LEFT_PAREN},
       Token{"int", token_t::SYMBOL},
       Token{"a", token_t::SYMBOL},
       Token{"", token_t::COMMA},
       Token{"int", token_t::SYMBOL},
       Token{"b", token_t::SYMBOL},
       Token{"", token_t::COMMA},
       Token{"int", token_t::SYMBOL},
       Token{"c", token_t::SYMBOL},
       Token{"", token_t::RIGHT_PAREN},
       Token{"", token_t::LEFT_BRACE},
       Token{"", token_t::IF},
       Token{"", token_t::LEFT_PAREN},
       Token{"true", token_t::SYMBOL},
       Token{"", token_t::RIGHT_PAREN},
       Token{"", token_t::LEFT_BRACE},
       Token{"int", token_t::SYMBOL},
       Token{"variable-0", token_t::SYMBOL},
       Token{"", token_t::ASSIGN},
       Token{"123", token_t::NUM},
       Token{"", token_t::SEMICOLON},
       Token{"", token_t::RIGHT_BRACE},
       Token{"", token_t::ELSE},
       Token{"", token_t::LEFT_BRACE},
       Token{"string", token_t::SYMBOL},
       Token{"literal-1", token_t::SYMBOL},
       Token{"", token_t::ASSIGN},
       Token{"Lorem ipsum", token_t::STRING_LITERAL},
       Token{"", token_t::SEMICOLON},
       Token{"", token_t::RIGHT_BRACE},
       Token{"", token_t::RIGHT_BRACE}});
}

std::string random_bytes(size_t count) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(20, 200);

  std::vector<int> bytes;
  for (size_t i = count; i > 0; i--) {
    bytes.push_back(distrib(gen));
  }

  std::stringstream ss;
  std::copy(bytes.begin(), bytes.end(), std::ostream_iterator<char>(ss, " "));

  return ss.str();
}

void lexer_fuzz_tests() {
  // Bad parsing errors
  lexer_detail::assert_exception("1____");
  lexer_detail::assert_exception("1.");
  lexer_detail::assert_exception("1..1");
  lexer_detail::assert_exception("?");
  lexer_detail::assert_exception("++++\\+++");
  lexer_detail::assert_exception("\"");
  lexer_detail::assert_exception("\'");
  // Unsupported char sequences
  lexer_detail::assert_exception("!@#$%^&*()*&^%$#@!");
  lexer_detail::assert_exception("日本語テキスト");
  lexer_detail::assert_exception("Texte français");
  lexer_detail::assert_exception("Texto en español");
  lexer_detail::assert_exception("Текст на русском языке");
  lexer_detail::assert_exception("Türkçe metin");
  lexer_detail::assert_exception(R"(
        Als glückliche Bestimmung gilt es mir heute, daß das
        Schicksal mir zum Geburtsort gerade Braunau am Inn
        zuwies. Liegt doch dieses Städtchen an der Grenze jener
        zwei deutschen Staaten, deren Wiedervereinigung mindestens uns
        Jüngeren als eine mit allen Mitteln durchzuführende Lebensaufgabe erscheint!
    )");

  lexer_detail::assert_exception(random_bytes(100).data());
  lexer_detail::assert_exception(random_bytes(1000).data());
  lexer_detail::assert_exception(random_bytes(2000).data());
  lexer_detail::assert_exception(random_bytes(3000).data());
}

void lexer_speed_tests() {
  std::string data =
      "void f(int a, int b, int c) {"
      "  if (true) {"
      "    int variable_0 = 123;"
      "  } else {"
      "    string literal_1 = \"Lorem ipsum\";"
      "  }"
      "}";

  /// Exponential grow
  for (size_t i = 0; i < 10; i++)
    data += data;

  Lexer lexer(std::istringstream{data.data()});

  std::cout << "\nLexer speed test - input size (" << data.size() / 1024.0 / 1024.0 << " MiB.)\n";
  speed_benchmark(1, [&lexer] {
    const std::vector<Token> tokens = lexer.tokenize();
  });
}

void run_lexer_tests() {
  std::cout << "Running lexer tests...\n====\n";

  lexer_number_literal_tests();
  lexer_string_literal_tests();
  lexer_symbol_tests();
  lexer_operator_tests();
  lexer_expression_tests();
  lexer_fuzz_tests();
  lexer_speed_tests();

  std::cout << "Lexer tests passed successfully\n";
}

#endif// WEAK_TESTS_LEXER_HPP
