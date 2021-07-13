#ifndef WEAK_TESTS_LEXER_HPP
#define WEAK_TESTS_LEXER_HPP

#include "../lexer/lexer.hpp"
#include "../tests/test_utility.hpp"

#include <sstream>
#include <iomanip>
#include <random>

namespace lexer_detail {

void run_test(std::string_view data, std::vector<Lexeme> assertion_lexemes)
{
    Lexer lexer(std::istringstream{data.data()});

    const std::vector<Lexeme> lexemes = lexer.tokenize();

    assertion_lexemes.emplace_back(Lexeme{"", token_t::end_of_data});

    assert(lexemes.size() == assertion_lexemes.size());

    for (std::size_t i = 0; i < lexemes.size(); i++)
    {
        if (lexemes[i].type != assertion_lexemes[i].type)
            throw LexicalError(dispatch_token(lexemes[i].type) + " got, but " + dispatch_token(assertion_lexemes[i].type) + " required");

        if (lexemes[i].data != assertion_lexemes[i].data)
            throw LexicalError(lexemes[i].data + " got, but " + assertion_lexemes[i].data + " required");
    }
}

void assert_exception(std::string_view data)
{
    try
    {
        Lexer lexer(std::istringstream{data.data()});
        lexer.tokenize();
    }
    catch (LexicalError& error) {
        std::cout << std::setw(25) << "Lexical error processed: " << error.what() << '\n';
        return;
    }
    catch (std::exception& error) {
        std::cout << std::setw(25) <<  "Error processed: " << error.what() << '\n';
        return;
    }

    [[maybe_unused]] const bool error_expected = false;

//    assert(error_expected);
}
} // namespace lexer_detail

void lexer_number_literal_tests()
{
    lexer_detail::run_test("2 2", {
        Lexeme{"2", token_t::num},
        Lexeme{"2", token_t::num}
    });
    lexer_detail::run_test("2 2 ", {
        Lexeme{"2", token_t::num},
        Lexeme{"2", token_t::num}
    });
    lexer_detail::run_test(" 2 2", {
        Lexeme{"2", token_t::num},
        Lexeme{"2", token_t::num}
    });
    lexer_detail::run_test("        2       2       ", {
        Lexeme{"2", token_t::num},
        Lexeme{"2", token_t::num}
    });
    lexer_detail::run_test("111.111", {
        Lexeme{"111.111", token_t::floating_point},
    });
    lexer_detail::run_test("111.111 222.222", {
        Lexeme{"111.111", token_t::floating_point},
        Lexeme{"222.222", token_t::floating_point}
    });
    lexer_detail::run_test(" 111.111 222.222 333.333 444.444 555.555 ", {
        Lexeme{"111.111", token_t::floating_point},
        Lexeme{"222.222", token_t::floating_point},
        Lexeme{"333.333", token_t::floating_point},
        Lexeme{"444.444", token_t::floating_point},
        Lexeme{"555.555", token_t::floating_point},
    });

    lexer_detail::assert_exception("1.");
    lexer_detail::assert_exception("1........");
    lexer_detail::assert_exception("1..2");
}

void lexer_string_literal_tests()
{
    lexer_detail::run_test("\"\"", {
        Lexeme{"", token_t::string_literal}
    });
    lexer_detail::run_test("\"text\"", {
       Lexeme{"text", token_t::string_literal}
    });
    lexer_detail::run_test("\"111.222\"", {
        Lexeme{"111.222", token_t::string_literal}
    });
    lexer_detail::run_test("\"text \\\" with escaped character \"", {
        Lexeme{"text \" with escaped character ", token_t::string_literal}
    });
    lexer_detail::run_test("\"Текст на русском языке\"", {
        Lexeme{"Текст на русском языке", token_t::string_literal}
    });
    lexer_detail::run_test("\"Türkçe metin\"", {
        Lexeme{"Türkçe metin", token_t::string_literal}
    });
    lexer_detail::run_test("\"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f\"", {
        Lexeme{"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f", token_t::string_literal}
    });
    lexer_detail::run_test("\" \\\"\"", {
        Lexeme{" \"", token_t::string_literal}
    });
    lexer_detail::run_test("\"\\\"", {
        Lexeme{"\\", token_t::string_literal}
    });
    lexer_detail::run_test("\" \\\\\"", {
        Lexeme{" \\", token_t::string_literal}
    });
    lexer_detail::run_test("\" \"", {
        Lexeme{" ", token_t::string_literal}
    });
    lexer_detail::run_test(" \"?\\\"\\\"\" \"?\\\"\\\"\\\"\" ", {
        Lexeme{"?\"\"", token_t::string_literal},
        Lexeme{"?\"\"\"", token_t::string_literal}
    });

    lexer_detail::assert_exception("\"text without closing quote");
    lexer_detail::assert_exception("\"\\");
}

void lexer_symbol_tests()
{
    lexer_detail::run_test("Symbol", {
        Lexeme{"Symbol", token_t::symbol}
    });
    lexer_detail::run_test("A B C", {
        Lexeme{"A", token_t::symbol},
        Lexeme{"B", token_t::symbol},
        Lexeme{"C", token_t::symbol}
    });
    lexer_detail::run_test("a1b2c3d4 a000000a", {
        Lexeme{"a1b2c3d4", token_t::symbol},
        Lexeme{"a000000a", token_t::symbol}
    });
    lexer_detail::run_test("     a1b2c3d4      a000000a       ", {
        Lexeme{"a1b2c3d4", token_t::symbol},
        Lexeme{"a000000a", token_t::symbol}
    });
    lexer_detail::run_test("test?", {
        Lexeme{"test?", token_t::symbol}
    });

    lexer_detail::assert_exception("1A");
    lexer_detail::assert_exception("0_0");
}

void lexer_operator_tests()
{
    lexer_detail::run_test("((((((((((", std::vector<Lexeme>(10, Lexeme{"", token_t::left_paren}));
    lexer_detail::run_test("))))))))))", std::vector<Lexeme>(10, Lexeme{"", token_t::right_paren}));

    std::string increments(200, '+');
    lexer_detail::run_test(increments, std::vector<Lexeme>(100, Lexeme{"", token_t::inc}));

    std::string decrements(200, '-');
    lexer_detail::run_test(decrements, std::vector<Lexeme>(100, Lexeme{"", token_t::dec}));

    lexer_detail::run_test("!", {
        Lexeme{"", token_t::negation},
    });
    lexer_detail::run_test("==", {
        Lexeme{"", token_t::eq},
    });
    lexer_detail::run_test("!=", {
        Lexeme{"", token_t::neq},
    });
    lexer_detail::run_test("+", {
        Lexeme{"", token_t::plus},
    });
    lexer_detail::run_test("++", {
        Lexeme{"", token_t::inc},
    });
    lexer_detail::run_test("+++", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::plus}
    });
    lexer_detail::run_test("++++", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::inc},
    });
    lexer_detail::run_test("+++++", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::plus}
    });
    lexer_detail::run_test("++ ++ +", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::plus}
    });
    lexer_detail::run_test("+ ++ + +", {
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::plus}
    });
    lexer_detail::run_test("+ += /=", {
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::plus_assign},
        Lexeme{"", token_t::slash_assign}
    });
    lexer_detail::run_test("++=/=", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::assign},
        Lexeme{"", token_t::slash_assign}
    });
    lexer_detail::run_test("...,,,", {
        Lexeme{"", token_t::dot},
        Lexeme{"", token_t::dot},
        Lexeme{"", token_t::dot},
        Lexeme{"", token_t::comma},
        Lexeme{"", token_t::comma},
        Lexeme{"", token_t::comma}
    });
    lexer_detail::run_test("++--++--+-+-++--+++---+", {
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::dec},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::dec},
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::minus},
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::minus},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::dec},
        Lexeme{"", token_t::inc},
        Lexeme{"", token_t::plus},
        Lexeme{"", token_t::dec},
        Lexeme{"", token_t::minus},
        Lexeme{"", token_t::plus},
    });
    lexer_detail::run_test("\0", {
        // None
    });

    // Unknown operators
    lexer_detail::assert_exception("+++`+++");
    lexer_detail::assert_exception("@@@@@@@");
}

void lexer_expression_tests()
{
    lexer_detail::run_test(
        "void f(int a, int b, int c) {"
        "  if (true) {"
        "    int variable-0 = 123;"
        "  } else {"
        "    string literal-1 = \"Lorem ipsum\";"
        "  }"
        "}",
    {
        Lexeme{"void",      token_t::symbol},
        Lexeme{"f",         token_t::symbol},
        Lexeme{"",          token_t::left_paren},
        Lexeme{"int",       token_t::symbol},
        Lexeme{"a",         token_t::symbol},
        Lexeme{"",          token_t::comma},
        Lexeme{"int",       token_t::symbol},
        Lexeme{"b",         token_t::symbol},
        Lexeme{"",          token_t::comma},
        Lexeme{"int",       token_t::symbol},
        Lexeme{"c",         token_t::symbol},
        Lexeme{"",          token_t::right_paren},
        Lexeme{"",          token_t::left_brace},
        Lexeme{"",          token_t::kw_if},
        Lexeme{"",          token_t::left_paren},
        Lexeme{"true",      token_t::symbol},
        Lexeme{"",          token_t::right_paren},
        Lexeme{"",          token_t::left_brace},
        Lexeme{"int",       token_t::symbol},
        Lexeme{"variable-0",token_t::symbol},
        Lexeme{"",          token_t::assign},
        Lexeme{"123",       token_t::num},
        Lexeme{"",          token_t::semicolon},
        Lexeme{"",          token_t::right_brace},
        Lexeme{"",          token_t::kw_else},
        Lexeme{"",          token_t::left_brace},
        Lexeme{"string",    token_t::symbol},
        Lexeme{"literal-1", token_t::symbol},
        Lexeme{"",          token_t::assign},
        Lexeme{"Lorem ipsum", token_t::string_literal},
        Lexeme{"",          token_t::semicolon},
        Lexeme{"",          token_t::right_brace},
        Lexeme{"",          token_t::right_brace}
    });
}

std::string random_bytes(std::size_t count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(20, 200);

    std::vector<int> bytes;
    for (std::size_t i = count; i > 0; i--)
    {
        bytes.push_back(distrib(gen));
    }

    std::stringstream ss;
    std::copy(bytes.begin(), bytes.end(), std::ostream_iterator<char>(ss, " "));

    return ss.str();
}

void lexer_fuzz_tests()
{
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

void lexer_speed_tests()
{
    std::string data =
        "void f(int a, int b, int c) {"
        "  if (true) {"
        "    int variable_0 = 123;"
        "  } else {"
        "    string literal_1 = \"Lorem ipsum\";"
        "  }"
        "}";

    /// Exponential grow
    for (std::size_t i = 0; i < 10; i++)
        data += data;

    Lexer lexer(std::istringstream{data.data()});

    std::cout << "\nLexer speed test - input size (" << data.size() / 1024.0 / 1024.0 << " MiB.)\n";
    speed_benchmark(1, [&lexer]{
        const std::vector<Lexeme> lexemes = lexer.tokenize();
    });
}

void run_lexer_tests()
{
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

#endif // WEAK_TESTS_LEXER_HPP
