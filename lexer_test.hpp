#ifndef LEXER_TEST_HPP
#define LEXER_TEST_HPP

#include <cassert>

#include "lexer.hpp"

namespace lexer_detail {

void run_test(std::string_view data, std::vector<Lexeme> assertion_lexemes)
{
    Lexer lexer(std::istringstream{data.data()});
    std::vector<Lexeme> lexemes = lexer.tokenize();

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
        Lexer{std::istringstream{data.data()}}.tokenize();
    }
    catch (std::exception& error) {
        assert(true);
        return;
    }

    assert(false);
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

void lexer_string_literal_test()
{
    lexer_detail::run_test("\"text\"", {
       Lexeme{"text", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"111.222\"", {
        Lexeme{"111.222", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"text \\\" with escaped sequence \"", {
        Lexeme{"text \" with escaped sequence ", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f\"", {
        Lexeme{"\n\r\v\f\n\r\v\f\n\r\v\f\n\r\v\f", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \\\"\"", {
        Lexeme{" \"", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \\\\\"", {
        Lexeme{" \\", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\"\\\"", {
        Lexeme{"\\", lexeme_t::string_literal}
    });
    lexer_detail::run_test("\" \"", {
        Lexeme{" ", lexeme_t::string_literal}
    });
    lexer_detail::run_test(" \"?\\\"\\\"\" \"?\\\"\\\"\\\"\" ", {
        Lexeme{"?\"\"", lexeme_t::string_literal},
        Lexeme{"?\"\"\"", lexeme_t::string_literal}
    });

    lexer_detail::assert_exception("\"text without closing quote");
}

void lexer_symbol_test()
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

void lexer_operator_test()
{
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
}

#endif //LEXER_TEST_HPP
