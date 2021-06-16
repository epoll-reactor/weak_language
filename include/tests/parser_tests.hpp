#ifndef PARSER_TESTS_HPP
#define PARSER_TESTS_HPP

#include <cassert>

#include "../lexer/lexer.hpp"
#include "../lexer/lexer_builder.hpp"
#include "../parser/parser.hpp"
#include "../tests/test_utility.hpp"
#include "../tests/grammar.hpp"

namespace parser_detail {

void run_test(std::string_view data, std::vector<std::shared_ptr<expression::Object>> assertion_trees)
{
    Lexer lexer = LexerBuilder{}
        .operators(test_operators)
        .keywords(test_keywords)
        .input(std::istringstream{data.data()})
        .build();

    Parser parser(lexer.tokenize());

    const std::shared_ptr<expression::RootObject> parsed_trees = parser.parse();

    for (std::size_t i = 0; i < parsed_trees->get().size(); i++)
    {
        tree_print(parsed_trees->get()[i]);
        std::cout << '\n';
        assert(parsed_trees->get()[i]->same_with(assertion_trees[i]));
    }
}

void run_block_test(std::string_view data, std::shared_ptr<expression::Block> assertion_block)
{
    Lexer lexer = LexerBuilder{}
        .operators(test_operators)
        .keywords(test_keywords)
        .input(std::istringstream{data.data()})
        .build();

    Parser parser(lexer.tokenize());

    for (const auto& object : parser.parse()->get())
    {
        auto block = std::dynamic_pointer_cast<expression::Block>(object);
        const auto& block_statements = block->statements();

        assert(assertion_block->statements().size() == block_statements.size());

        for (std::size_t i = 0; i < block_statements.size(); i++)
        {
            assert(block_statements[i]->same_with(assertion_block->statements()[i]));
        }
    }
}

void assert_correct(std::string_view data)
{
    Lexer lexer = LexerBuilder{}
        .operators(test_operators)
        .keywords(test_keywords)
        .input(std::istringstream{data.data()})
        .build();

    try
    {
        Parser parser(lexer.tokenize());
        parser.parse();

    } catch (LexicalError& lex_error) {

        std::cout << "While analyzing:\n\t" << data << "\nLexical error processed:\n\t" << lex_error.what() << "\n\n";

    } catch (ParseError& parse_error) {

        std::cout << "While analyzing:\n\t" << data << "\nParse error processed:\n\t" << parse_error.what() << "\n\n";
    }

}

} // namespace parser_detail

void run_parser_tests()
{
    std::cout << "Running parser tests...\n====\n";

    auto alloc_num = [](std::string_view data) {
        return std::make_shared<expression::Number>(data.data());
    };
    auto alloc_symbol = [](std::string_view data) {
        return std::make_shared<expression::Symbol>(data.data());
    };
    auto alloc_string = [](std::string_view data) {
        return std::make_shared<expression::String>(data.data());
    };
    auto alloc_binary = [](lexeme_t type, auto lhs, auto rhs) {
        return std::make_shared<expression::Binary>(type, lhs, rhs);
    };
    auto alloc_block = [](std::vector<std::shared_ptr<expression::Object>> expressions) {
        return std::make_shared<expression::Block>(std::move(expressions));
    };
    auto alloc_if = [](std::shared_ptr<expression::Object> exit_condition, std::shared_ptr<expression::Block> body) {
        return std::make_shared<expression::If>(std::move(exit_condition), std::move(body));
    };
    auto alloc_if_else = [](std::shared_ptr<expression::Object> exit_condition, std::shared_ptr<expression::Block> if_body, std::shared_ptr<expression::Block> else_body) {
        return std::make_shared<expression::If>(std::move(exit_condition), std::move(if_body), std::move(else_body));
    };
    auto alloc_while = [](std::shared_ptr<expression::Object> exit_condition, std::shared_ptr<expression::Block> body) {
        return std::make_shared<expression::While>(std::move(exit_condition), std::move(body));
    };

    parser_detail::run_test("1;", {{
        alloc_num("1")
    }});

    parser_detail::run_test("1;2;3;", {
        {alloc_num("1")},{alloc_num("2")},{alloc_num("3")}
    });

    parser_detail::run_test("\"123\";", {{
        alloc_string("123")
    }});

    parser_detail::run_test("a = 1;", {{
        alloc_binary(
            lexeme_t::assign,
            alloc_symbol("a"),
            alloc_num("1")
        )
    }});

    parser_detail::run_test("2 + 3 + 4;", {{
        alloc_binary(
            lexeme_t::plus,
            alloc_num("2"),
            alloc_binary(
                lexeme_t::plus,
                alloc_num("3"),
                alloc_num("4")
           )
        )
    }});

    parser_detail::run_test("2 + 3 * 4;", {{
       alloc_binary(
           lexeme_t::plus,
           alloc_num("2"),
           alloc_binary(
               lexeme_t::star,
               alloc_num("3"),
               alloc_num("4")
           )
       )
   }});

    /// {1 + {{2 * 3} + 4}}
//    parser_detail::run_test("1 + 2 * 3 + 4;", {{
//       alloc_binary(
//           lexeme_t::plus,
//           alloc_num("1"),
//           alloc_binary(
//               lexeme_t::plus,
//               alloc_binary(
//                   lexeme_t::star,
//                   alloc_num("2"),
//                   alloc_num("3")
//               ),
//               alloc_num("4")
//           )
//       )
//   }});

    parser_detail::run_test("1 + 2 + 3 + 4 + 5 + 6;", {{
       alloc_binary(
           lexeme_t::plus,
           alloc_num("1"),
           alloc_binary(
               lexeme_t::plus,
               alloc_num("2"),
               alloc_binary(
                   lexeme_t::plus,
                   alloc_num("3"),
                   alloc_binary(
                       lexeme_t::plus,
                       alloc_num("4"),
                       alloc_binary(
                           lexeme_t::plus,
                           alloc_num("5"),
                           alloc_num("6")
                       )
                   )
               )
           )
       )
    }});
    /// Actually wrong parsing
    parser_detail::run_test("1 + 2 * 3 + 4; 2 + 5;", {{
        alloc_binary(
            lexeme_t::plus,
            alloc_num("1"),
            alloc_binary(
                lexeme_t::star,
                alloc_num("2"),
                alloc_binary(
                    lexeme_t::plus,
                    alloc_num("3"),
                    alloc_num("4")
                )
            )
        )
    }, {
        alloc_binary(
            lexeme_t::plus,
            alloc_num("2"),
            alloc_num("5")
        )
    }});

    parser_detail::run_test("a + b + 2 + z + 3 + y + 4 + z;", {{
       alloc_binary(
           lexeme_t::plus,
           alloc_symbol("a"),
           alloc_binary(
               lexeme_t::plus,
               alloc_symbol("b"),
               alloc_binary(
                   lexeme_t::plus,
                   alloc_num("2"),
                   alloc_binary(
                       lexeme_t::plus,
                       alloc_symbol("z"),
                       alloc_binary(
                           lexeme_t::plus,
                           alloc_num("3"),
                           alloc_binary(
                               lexeme_t::plus,
                               alloc_symbol("y"),
                               alloc_binary(
                                   lexeme_t::plus,
                                   alloc_num("4"),
                                   alloc_symbol("z")
                               )
                           )
                       )
                   )
               )
           )
       )
    }});

    parser_detail::run_block_test(
        "{"
        "   1 + 1;"
        "   2 + 2;"
        "}",
    {
        alloc_block({{
            alloc_binary(
                lexeme_t::plus,
                alloc_num("1"),
                alloc_num("1")
            )
        }, {
            alloc_binary(
                lexeme_t::plus,
                alloc_num("2"),
                alloc_num("2")
            )
        }})
    });

    parser_detail::run_block_test(
        "{"
        "   {"
        "       1 + 1;"
        "   }"
        "}",
    {
        alloc_block({
            alloc_block({
                alloc_binary(
                    lexeme_t::plus,
                    alloc_num("1"),
                    alloc_num("1")
                )
            })
        })
    });

    parser_detail::run_block_test(
        "{"
        "   {"
        "       {"
        "           {"
        "               \"So deep\";"
        "           }"
        "       }"
        "   }"
        "}",
    {
        alloc_block({
            alloc_block({
                alloc_block({
                    alloc_block({
                        alloc_string({
                            "So deep"
                        })
                    })
                })
            })
        })
    });

    parser_detail::run_block_test(
        "{                          "
        "   {                       "
        "       \"Lorem ipsum\";    "
        "   }                       "
        "   \"Lorem ipsum 2\";      "
        "   \"Lorem ipsum 3\";      "
        "   {                       "
        "       1 + 1;              "
        "   }                       "
        "}                          ",
    {
        alloc_block({
            alloc_block({
                alloc_string("Lorem ipsum")
            }),
            alloc_string("Lorem ipsum 2"),
            alloc_string("Lorem ipsum 3"),
            alloc_block({
                alloc_binary(
                    lexeme_t::plus,
                    alloc_num("1"),
                    alloc_num("1")
                )
            })
        })
    });

    parser_detail::run_test(
        "if (1 == 2)"
        "{"
        "   3 + 4;"
        "   {"
        "       \"Block\";"
        "   }"
        "}",
        {
            alloc_if(
                alloc_binary(
                    lexeme_t::equal,
                    alloc_num("1"),
                    alloc_num("2")
                ),
                alloc_block({
                    alloc_binary(
                        lexeme_t::plus,
                        alloc_num("3"),
                        alloc_num("4")
                    ),
                    alloc_block({
                        alloc_string("Block")
                    })
                })
            )
        });

    parser_detail::run_test(
        "if (1 == 2)"
        "{"
        "   if (0 == 0)"
        "   {"
        "       if (1)"
        "       {"
        "           1 + 2;"
        "       }"
        "   }"
        "}",
    {
        alloc_if(
            alloc_binary(
                lexeme_t::equal,
                alloc_num("1"),
                alloc_num("2")
            ),
            alloc_block({
                alloc_if(
                    alloc_binary(
                        lexeme_t::equal,
                        alloc_num("0"),
                        alloc_num("0")
                    ),
                    alloc_block({
                        alloc_if(
                            alloc_num("1"),
                            alloc_block({
                                alloc_binary(
                                    lexeme_t::plus,
                                    alloc_num("1"),
                                    alloc_num("2")
                                )
                            })
                        )
                    })
                )
            })
        )
    });

    parser_detail::run_test(
        "if (1 == 1)"
        "{"
        "   1;"
        "}"
        "else {"
        "   2;"
        "}",
    {
        alloc_if_else(
            alloc_binary(
                lexeme_t::equal,
                alloc_num("1"),
                alloc_num("1")
            ),
            /// If body
            alloc_block({
                alloc_num("1")
            }),
            /// Else body
            alloc_block({
                alloc_num("2")
            })
        )
    });

    parser_detail::run_test(
        "while (1)"
        "{"
        "   if (0)"
        "   {"
        "       \"Inside while\";"
        "   }"
        "}",
    {
        alloc_while(
            alloc_num("1"),
            alloc_block({
                alloc_if(
                    alloc_num("0"),
                    alloc_block({
                        alloc_string("Inside while")
                    })
                )
            })
        )
    });

    parser_detail::assert_correct("");
    parser_detail::assert_correct("{}");
    parser_detail::assert_correct("{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}");
    parser_detail::assert_correct("if (1) {} else {}");
    parser_detail::assert_correct("if (1) {} else { if (2) {} else { if (3) {} else {} } }");
    parser_detail::assert_correct(R"__(
        if (external_symbol) {
            {
               string_template = "Lorem ipsum";
               number_template = 000000000000000;
            }
            if (0)
            {
                complex_template = 1 - 1 - 1 - 1 - 1 - 1 - 1 - 1 - 1;
            }
            else
            {
                {
                    while ("1")
                    {
                        {
                            inside_block = "0";
                        }
                    }
                }
            }
        })__"
    );

    std::cout << "Parser tests passed successfully\n";
}

#endif // PARSER_TESTS_HPP
