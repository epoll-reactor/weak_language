#ifndef PARSE_OBJECT_TESTS_HPP
#define PARSE_OBJECT_TESTS_HPP

#include <cassert>
#include <iomanip>

namespace parse_object_detail {

template <typename Function>
void assert_exception(Function&& test_body)
{
    try
    {
        test_body();
    }
    catch (ParseError& error) {
        std::cout << std::setw(25) << "Parse error handled: " << error.what() << '\n';
        return;
    }
    catch (std::exception& error) {
        std::cout << std::setw(25) << "Error handled: " << error.what() << '\n';
        return;
    }

    bool error_expected = false;

    assert(error_expected);
}
}

void parse_object_number_test()
{
    std::shared_ptr<expression::Number> integral = std::make_shared<expression::Number>("123");
    std::shared_ptr<expression::Number> floating_point = std::make_shared<expression::Number>("123.123");

    assert(integral->value() == 123);
    assert(floating_point->value() == 123.123);
}

void parse_object_number_fuzz_test()
{
    parse_object_detail::assert_exception([](){
        std::make_shared<expression::Number>("Text, but number expected");
    });
    parse_object_detail::assert_exception([](){
        std::make_shared<expression::Number>("a11111");
    });
}

void parse_object_string_test()
{
    std::shared_ptr<expression::String> string = std::make_shared<expression::String>("Text");

    assert(string->value() == "Text");
}

void parse_object_binary_operation_test()
{
    std::shared_ptr<expression::Number> number = std::make_shared<expression::Number>("1");

    std::shared_ptr<expression::Binary> bin = std::make_shared<expression::Binary>(
        lexeme_t::plus,
        number,
        number
    );

    assert(bin->type() == lexeme_t::plus);
    assert(std::dynamic_pointer_cast<expression::Number>(bin->lhs()));
    assert(std::dynamic_pointer_cast<expression::Number>(bin->rhs()));
    assert(std::dynamic_pointer_cast<expression::Number>(bin->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<expression::Number>(bin->lhs())->value() == 1);

    auto assign_object = std::make_shared<expression::Binary>(
        lexeme_t::assign,
        std::make_shared<expression::Symbol>("Name"),
        std::make_shared<expression::Binary>(
            lexeme_t::plus,
            std::make_shared<expression::Number>("100"),
            std::make_shared<expression::Number>("200")
        )
    );

    assert(assign_object->type() == lexeme_t::assign);
    assert(std::dynamic_pointer_cast<expression::Symbol>(assign_object->lhs())->name() == "Name");
    assert(std::dynamic_pointer_cast<expression::Binary>(assign_object->rhs())->type() == lexeme_t::plus);
}

void parse_object_unary_operation_test()
{
    std::shared_ptr<expression::Number> number = std::make_shared<expression::Number>("1");

    std::shared_ptr<expression::Unary> unary = std::make_shared<expression::Unary>(
        lexeme_t::inc,
        number
    );

    assert(unary->type() == lexeme_t::inc);
    assert(std::dynamic_pointer_cast<expression::Number>(unary->operand())->value() == 1);
}

void parse_object_nested_expression_test()
{
    std::shared_ptr<expression::Binary> bin = std::make_shared<expression::Binary>(
        lexeme_t::plus,
        std::make_shared<expression::Number>("1"),
        std::make_shared<expression::Number>("2")
    );

    std::shared_ptr<expression::Binary> nested_bin = std::make_shared<expression::Binary>(
        lexeme_t::minus,
        bin,
        bin
    );

    auto left_node = std::dynamic_pointer_cast<expression::Binary>(nested_bin->lhs());
    auto right_node = std::dynamic_pointer_cast<expression::Binary>(nested_bin->rhs());

    assert(std::dynamic_pointer_cast<expression::Number>(left_node->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<expression::Number>(left_node->rhs())->value() == 2);
    assert(std::dynamic_pointer_cast<expression::Number>(right_node->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<expression::Number>(right_node->rhs())->value() == 2);
}

void parse_object_deep_nested_expression_test(std::size_t tree_depth)
{
    std::shared_ptr<expression::Unary> unary = std::make_shared<expression::Unary>(
        lexeme_t::plus,
        std::make_shared<expression::Symbol>("pthread_create@@GLIBC_2.2.5")
    );

    for (std::size_t i = 0; i < tree_depth; i++)
    {
        unary = std::make_shared<expression::Unary>(
            lexeme_t::plus,
            std::shared_ptr<expression::Unary>(unary)
        );
    }

    for (std::size_t i = 0; i < tree_depth; i++)
    {
        assert(unary->type() == lexeme_t::plus);
        unary = std::dynamic_pointer_cast<expression::Unary>(unary->operand());
    }

    assert(std::dynamic_pointer_cast<expression::Symbol>(unary->operand())->name() == "pthread_create@@GLIBC_2.2.5");

    std::cout << "Nested expression test of depth = " << tree_depth << " passed\n";
}

void parse_object_while_tests()
{
    auto exit_condition = std::make_shared<expression::Binary>(
        lexeme_t::equal,
        std::make_shared<expression::Number>("1"),
        std::make_shared<expression::Number>("1")
    );

    auto while_body = std::make_shared<expression::Block>(
        std::vector<std::shared_ptr<expression::Object>>{{
            std::make_shared<expression::Binary>(
                lexeme_t::assign,
                std::make_shared<expression::Symbol>("Symbol"),
                std::make_shared<expression::Number>("1")
            )
        }, {
            std::make_shared<expression::Binary>(
                lexeme_t::assign,
                std::make_shared<expression::Symbol>("Symbol2"),
                std::make_shared<expression::Number>("2")
            )
        }, {
            std::make_shared<expression::Unary>(
                lexeme_t::inc,
                std::make_shared<expression::Symbol>("Symbol2")
            )
        }}
    );

    auto while_object = std::make_shared<expression::While>(exit_condition, while_body);

    tree_print(while_object);

    assert(while_object->exit_condition()->same_with(exit_condition));
    assert(while_object->body()->same_with(while_body));
}

void parse_object_if_tests()
{
    auto if_block = std::make_shared<expression::If>(
        std::make_shared<expression::Binary>(
            lexeme_t::equal,
            std::make_shared<expression::String>("1"),
            std::make_shared<expression::String>("1")
        ),
        /// If body
        std::make_shared<expression::Block>(std::vector<std::shared_ptr<expression::Object>>{
            std::make_shared<expression::String>("1")
        })
    );

    assert(!if_block->else_body());
}

void parse_object_if_else_tests()
{
    auto if_block = std::make_shared<expression::If>(
        std::make_shared<expression::Binary>(
            lexeme_t::equal,
            std::make_shared<expression::String>("1"),
            std::make_shared<expression::String>("1")
        ),
        /// If body
        std::make_shared<expression::Block>(std::vector<std::shared_ptr<expression::Object>>{
            std::make_shared<expression::String>("1")
        }),
        /// Else body
        std::make_shared<expression::Block>(std::vector<std::shared_ptr<expression::Object>>{
            std::make_shared<expression::String>("1")
        })
    );

    assert(if_block->else_body());
}

void run_expression_tests()
{
    std::cout << "Running parse tree tests...\n====\n";

    parse_object_number_test();
    parse_object_number_fuzz_test();
    parse_object_string_test();
    parse_object_binary_operation_test();
    parse_object_unary_operation_test();
    parse_object_nested_expression_test();
    parse_object_while_tests();
    parse_object_if_tests();
    parse_object_if_else_tests();
    parse_object_deep_nested_expression_test(10);
    parse_object_deep_nested_expression_test(100);
    parse_object_deep_nested_expression_test(1000);

    std::cout << "Parse tree tests passed successfully\n";
}

#endif // PARSE_OBJECT_TESTS_HPP