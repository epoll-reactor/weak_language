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

    const std::shared_ptr<expression::RootObject> parsed_trees = parser.parse();

    for (const auto& object : parsed_trees->get())
    {
        auto block = std::dynamic_pointer_cast<expression::Block>(object);
        const auto& block_statements = block->statements();

        tree_print(block);

        assert(assertion_block->statements().size() == block_statements.size());

        for (std::size_t i = 0; i < block_statements.size(); i++)
        {
            assert(block_statements[i]->same_with(assertion_block->statements()[i]));
        }

        break;
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

    parser_detail::run_test("1;", {{
        alloc_num("1")
    }});

    parser_detail::run_test("1;2;3;4;5;6;7;8;9;", {
        {alloc_num("1")},{alloc_num("2")},{alloc_num("3")},
        {alloc_num("4")},{alloc_num("5")},{alloc_num("6")},
        {alloc_num("7")},{alloc_num("8")},{alloc_num("9")}
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

    /// Nested blocks in process...
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

    std::cout << "Parser tests passed successfully\n";
}

#endif // PARSER_TESTS_HPP
