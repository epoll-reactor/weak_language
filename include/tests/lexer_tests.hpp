#ifndef LEXER_TESTS_HPP
#define LEXER_TESTS_HPP

#include <cassert>
#include <iomanip>
#include <cstring>
#include <iterator>
#include <random>

#include "../lexer/lexer.hpp"
#include "../lexer/lexer_builder.hpp"
#include "../tests/grammar.hpp"


namespace lexer_detail {

void run_test(std::string_view data, std::vector<Lexeme> assertion_lexemes)
{
    Lexer lexer = LexerBuilder{}
        .operators(test_operators)
        .keywords(test_keywords)
        .input(std::istringstream{data.data()})
        .build();

    const std::vector<Lexeme> lexemes = lexer.tokenize();

    assertion_lexemes.emplace_back(Lexeme{"", lexeme_t::end_of_data});

    assert(lexemes.size() == assertion_lexemes.size());

    for (std::size_t i = 0; i < lexemes.size(); i++)
    {
        assert(lexemes[i].type == assertion_lexemes[i].type);
        assert(lexemes[i].data == assertion_lexemes[i].data);
    }
}

void assert_exception(std::string_view data)
{
    try
    {
        LexerBuilder{}
            .operators(test_operators)
            .keywords(test_keywords)
            .input(std::istringstream{data.data()})
            .build()
            .tokenize();
    }
    catch (LexicalError& error) {
        std::cout << std::setw(25) << "Lexical error processed: " << error.what() << '\n';
        return;
    }
    catch (std::exception& error) {
        std::cout << std::setw(25) <<  "Error processed: " << error.what() << '\n';
        return;
    }

    const bool error_expected = false;

    assert(error_expected);
}
} // namespace lexer_detail

void lexer_number_literal_tests()
{
    lexer_detail::run_test("2 2", {
        Lexeme{"2", lexeme_t::num},
        Lexeme{"2", lexeme_t::num}
    });
    lexer_detail::run_test("2 2 ", {
        Lexeme{"2", lexeme_t::num},
        Lexeme{"2", lexeme_t::num}
    });
    lexer_detail::run_test(" 2 2", {
        Lexeme{"2", lexeme_t::num},
        Lexeme{"2", lexeme_t::num}
    });
    lexer_detail::run_test("        2       2       ", {
        Lexeme{"2", lexeme_t::num},
        Lexeme{"2", lexeme_t::num}
    });
    lexer_detail::run_test("111.111", {
        Lexeme{"111.111", lexeme_t::num},
    });
    lexer_detail::run_test("111.111 222.222", {
        Lexeme{"111.111", lexeme_t::num},
        Lexeme{"222.222", lexeme_t::num}
    });
    lexer_detail::run_test(" 111.111 222.222 333.333 444.444 555.555 ", {
        Lexeme{"111.111", lexeme_t::num},
        Lexeme{"222.222", lexeme_t::num},
        Lexeme{"333.333", lexeme_t::num},
        Lexeme{"444.444", lexeme_t::num},
        Lexeme{"555.555", lexeme_t::num},
    });

    lexer_detail::assert_exception("1.");
    lexer_detail::assert_exception("1........");
    lexer_detail::assert_exception("1..2");
}

void lexer_string_literal_tests()
{
    lexer_detail::run_test("\"\"", {
        Lexeme{"", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"text\"", {
       Lexeme{"text", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"111.222\"", {
        Lexeme{"111.222", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"text \\\" with escaped character \"", {
        Lexeme{"text \" with escaped character ", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"Текст на русском языке\"", {
        Lexeme{"Текст на русском языке", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"Türkçe metin\"", {
        Lexeme{"Türkçe metin", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f\"", {
        Lexeme{"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \\\"\"", {
        Lexeme{" \"", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"\\\"", {
        Lexeme{"\\", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \\\\\"", {
        Lexeme{" \\", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \"", {
        Lexeme{" ", lexeme_t::string_literal}
    });
    lexer_detail::run_test(" \"?\\\"\\\"\" \"?\\\"\\\"\\\"\" ", {
        Lexeme{"?\"\"", lexeme_t::string_literal},
        Lexeme{"?\"\"\"", lexeme_t::string_literal}
    });

    lexer_detail::assert_exception("\"text without closing quote");
    lexer_detail::assert_exception("\"\\");
}

void lexer_symbol_tests()
{
    lexer_detail::run_test("Symbol", {
        Lexeme{"Symbol", lexeme_t::symbol}
    });
    lexer_detail::run_test("A B C", {
        Lexeme{"A", lexeme_t::symbol},
        Lexeme{"B", lexeme_t::symbol},
        Lexeme{"C", lexeme_t::symbol}
    });
    lexer_detail::run_test("a1b2c3d4 a000000a", {
        Lexeme{"a1b2c3d4", lexeme_t::symbol},
        Lexeme{"a000000a", lexeme_t::symbol}
    });
    lexer_detail::run_test("     a1b2c3d4      a000000a       ", {
        Lexeme{"a1b2c3d4", lexeme_t::symbol},
        Lexeme{"a000000a", lexeme_t::symbol}
    });

    lexer_detail::assert_exception("1A");
    lexer_detail::assert_exception("0_0");
}

void lexer_operator_tests()
{
    lexer_detail::run_test("((((((((((", std::vector<Lexeme>(10, Lexeme{"", lexeme_t::left_paren}));
    lexer_detail::run_test("))))))))))", std::vector<Lexeme>(10, Lexeme{"", lexeme_t::right_paren}));

    std::string increments(200, '+');
    lexer_detail::run_test(increments, std::vector<Lexeme>(100, Lexeme{"", lexeme_t::inc}));

    std::string decrements(200, '-');
    lexer_detail::run_test(decrements, std::vector<Lexeme>(100, Lexeme{"", lexeme_t::dec}));

    lexer_detail::run_test("+", {
        Lexeme{"", lexeme_t::plus},
    });
    lexer_detail::run_test("++", {
        Lexeme{"", lexeme_t::inc},
    });
    lexer_detail::run_test("+++", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::plus}
    });
    lexer_detail::run_test("++++", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::inc},
    });
    lexer_detail::run_test("+++++", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::plus}
    });
    lexer_detail::run_test("++ ++ +", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::plus}
    });
    lexer_detail::run_test("+ ++ + +", {
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::plus}
    });
    lexer_detail::run_test("+ += /=", {
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::plus_assign},
        Lexeme{"", lexeme_t::slash_assign}
    });
    lexer_detail::run_test("++=/=", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::assign},
        Lexeme{"", lexeme_t::slash_assign}
    });
    lexer_detail::run_test("...,,,", {
        Lexeme{"", lexeme_t::dot},
        Lexeme{"", lexeme_t::dot},
        Lexeme{"", lexeme_t::dot},
        Lexeme{"", lexeme_t::comma},
        Lexeme{"", lexeme_t::comma},
        Lexeme{"", lexeme_t::comma}
    });
    lexer_detail::run_test("++--++--+-+-++--+++---+", {
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::dec},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::dec},
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::minus},
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::minus},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::dec},
        Lexeme{"", lexeme_t::inc},
        Lexeme{"", lexeme_t::plus},
        Lexeme{"", lexeme_t::dec},
        Lexeme{"", lexeme_t::minus},
        Lexeme{"", lexeme_t::plus},
    });
    lexer_detail::run_test("\0", {
        // None
    });

    // Unknown operators
    lexer_detail::assert_exception("<");
    lexer_detail::assert_exception("+++^+++");
    lexer_detail::assert_exception("@@@@@@@");
}

void lexer_expression_tests()
{
    lexer_detail::run_test(
        "void f(int a, int b, int c) {"
        "  if (true) {"
        "    int variable_0 = 123;"
        "  } else {"
        "    string literal_1 = \"Lorem ipsum\";"
        "  }"
        "}",
    {
        Lexeme{"void",      lexeme_t::symbol},
        Lexeme{"f",         lexeme_t::symbol},
        Lexeme{"",          lexeme_t::left_paren},
        Lexeme{"int",       lexeme_t::symbol},
        Lexeme{"a",         lexeme_t::symbol},
        Lexeme{"",          lexeme_t::comma},
        Lexeme{"int",       lexeme_t::symbol},
        Lexeme{"b",         lexeme_t::symbol},
        Lexeme{"",          lexeme_t::comma},
        Lexeme{"int",       lexeme_t::symbol},
        Lexeme{"c",         lexeme_t::symbol},
        Lexeme{"",          lexeme_t::right_paren},
        Lexeme{"",          lexeme_t::left_brace},
        Lexeme{"",          lexeme_t::kw_if},
        Lexeme{"",          lexeme_t::left_paren},
        Lexeme{"true",      lexeme_t::symbol},
        Lexeme{"",          lexeme_t::right_paren},
        Lexeme{"",          lexeme_t::left_brace},
        Lexeme{"int",       lexeme_t::symbol},
        Lexeme{"variable_0",lexeme_t::symbol},
        Lexeme{"",          lexeme_t::assign},
        Lexeme{"123",       lexeme_t::num},
        Lexeme{"",          lexeme_t::semicolon},
        Lexeme{"",          lexeme_t::right_brace},
        Lexeme{"",          lexeme_t::kw_else},
        Lexeme{"",          lexeme_t::left_brace},
        Lexeme{"string",    lexeme_t::symbol},
        Lexeme{"literal_1", lexeme_t::symbol},
        Lexeme{"",          lexeme_t::assign},
        Lexeme{"Lorem ipsum", lexeme_t::string_literal},
        Lexeme{"",          lexeme_t::semicolon},
        Lexeme{"",          lexeme_t::right_brace},
        Lexeme{"",          lexeme_t::right_brace}
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

void run_lexer_tests()
{
    std::cout << "Running lexer tests...\n====\n";

    lexer_number_literal_tests();
    lexer_string_literal_tests();
    lexer_symbol_tests();
    lexer_operator_tests();
    lexer_expression_tests();
    lexer_fuzz_tests();

    std::cout << "Lexer tests passed successfully\n";
}

#endif // LEXER_TESTS_HPP
