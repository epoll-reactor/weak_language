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

#ifdef AST_DEBUG
void parse_object_number_test()
{
    std::shared_ptr<ast::Number> integral = std::make_shared<ast::Number>("123");
    std::shared_ptr<ast::Number> floating_point = std::make_shared<ast::Number>("123.123");

    assert(integral->value() == 123);
    assert(floating_point->value() == 123.123);
}

void parse_object_array_test()
{
    std::vector<std::shared_ptr<ast::Object>> elements(100, std::make_shared<ast::Number>(1));

    auto array = std::make_shared<ast::Array>(std::move(elements));

    assert(array->elements().size() == 100);

    for (const auto& element : array->elements())
    {
        assert(element->same_with(std::make_shared<ast::Number>(1)));
    }
}

void parse_object_number_fuzz_test()
{
    parse_object_detail::assert_exception([](){
        std::make_shared<ast::Number>("Text, but number expected");
    });
    parse_object_detail::assert_exception([](){
        std::make_shared<ast::Number>("a11111");
    });
}

void parse_object_string_test()
{
    std::shared_ptr<ast::String> string = std::make_shared<ast::String>("Text");

    assert(string->value() == "Text");
}

void parse_object_binary_operation_test()
{
    std::shared_ptr<ast::Number> number = std::make_shared<ast::Number>("1");

    std::shared_ptr<ast::Binary> bin = std::make_shared<ast::Binary>(
        lexeme_t::plus,
        number,
        number
    );

    assert(bin->type() == lexeme_t::plus);
    assert(std::dynamic_pointer_cast<ast::Number>(bin->lhs()));
    assert(std::dynamic_pointer_cast<ast::Number>(bin->rhs()));
    assert(std::dynamic_pointer_cast<ast::Number>(bin->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<ast::Number>(bin->lhs())->value() == 1);

    auto assign_object = std::make_shared<ast::Binary>(
        lexeme_t::assign,
        std::make_shared<ast::Symbol>("Name"),
        std::make_shared<ast::Binary>(
            lexeme_t::plus,
            std::make_shared<ast::Number>("100"),
            std::make_shared<ast::Number>("200")
        )
    );

    assert(assign_object->type() == lexeme_t::assign);
    assert(std::dynamic_pointer_cast<ast::Symbol>(assign_object->lhs())->name() == "Name");
    assert(std::dynamic_pointer_cast<ast::Binary>(assign_object->rhs())->type() == lexeme_t::plus);
}

void parse_object_unary_operation_test()
{
    std::shared_ptr<ast::Number> number = std::make_shared<ast::Number>("1");

    std::shared_ptr<ast::Unary> unary = std::make_shared<ast::Unary>(
        lexeme_t::inc,
        number
    );

    assert(unary->type() == lexeme_t::inc);
    assert(std::dynamic_pointer_cast<ast::Number>(unary->operand())->value() == 1);
}

void parse_object_nested_expression_test()
{
    std::shared_ptr<ast::Binary> bin = std::make_shared<ast::Binary>(
        lexeme_t::plus,
        std::make_shared<ast::Number>("1"),
        std::make_shared<ast::Number>("2")
    );

    std::shared_ptr<ast::Binary> nested_bin = std::make_shared<ast::Binary>(
        lexeme_t::minus,
        bin,
        bin
    );

    auto left_node = std::dynamic_pointer_cast<ast::Binary>(nested_bin->lhs());
    auto right_node = std::dynamic_pointer_cast<ast::Binary>(nested_bin->rhs());

    assert(std::dynamic_pointer_cast<ast::Number>(left_node->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<ast::Number>(left_node->rhs())->value() == 2);
    assert(std::dynamic_pointer_cast<ast::Number>(right_node->lhs())->value() == 1);
    assert(std::dynamic_pointer_cast<ast::Number>(right_node->rhs())->value() == 2);
}

void parse_object_deep_nested_expression_test(std::size_t tree_depth)
{
    std::shared_ptr<ast::Unary> unary = std::make_shared<ast::Unary>(
        lexeme_t::plus,
        std::make_shared<ast::Symbol>("pthread_create@@GLIBC_2.2.5")
    );

    for (std::size_t i = 0; i < tree_depth; i++)
    {
        unary = std::make_shared<ast::Unary>(
            lexeme_t::plus,
            std::shared_ptr<ast::Unary>(unary)
        );
    }

    for (std::size_t i = 0; i < tree_depth; i++)
    {
        assert(unary->type() == lexeme_t::plus);
        unary = std::dynamic_pointer_cast<ast::Unary>(unary->operand());
    }

    assert(std::dynamic_pointer_cast<ast::Symbol>(unary->operand())->name() == "pthread_create@@GLIBC_2.2.5");

    std::cout << "Nested ast test of depth = " << tree_depth << " passed\n";
}

void parse_object_while_tests()
{
    auto exit_condition = std::make_shared<ast::Binary>(
        lexeme_t::eq,
        std::make_shared<ast::Number>("1"),
        std::make_shared<ast::Number>("1")
    );

    auto while_body = std::make_shared<ast::Block>(
        std::vector<std::shared_ptr<ast::Object>>{{
            std::make_shared<ast::Binary>(
                lexeme_t::assign,
                std::make_shared<ast::Symbol>("Symbol"),
                std::make_shared<ast::Number>("1")
            )
        },                                        {
            std::make_shared<ast::Binary>(
                lexeme_t::assign,
                std::make_shared<ast::Symbol>("Symbol2"),
                std::make_shared<ast::Number>("2")
            )
        },                                        {
            std::make_shared<ast::Unary>(
                lexeme_t::inc,
                std::make_shared<ast::Symbol>("Symbol2")
            )
        }}
    );

    auto while_object = std::make_shared<ast::While>(exit_condition, while_body);

    assert(while_object->exit_condition()->same_with(exit_condition));
    assert(while_object->body()->same_with(while_body));
}

void parse_object_if_tests()
{
    auto if_block = std::make_shared<ast::If>(
        std::make_shared<ast::Binary>(
            lexeme_t::eq,
            std::make_shared<ast::String>("1"),
            std::make_shared<ast::String>("1")
        ),
        /// If body
        std::make_shared<ast::Block>(std::vector<std::shared_ptr<ast::Object>>{
            std::make_shared<ast::String>("1")
        })
    );

    assert(!if_block->else_body());
}

void parse_object_if_else_tests()
{

    auto if_block = std::make_shared<ast::If>(
        std::make_shared<ast::Binary>(
            lexeme_t::eq,
            std::make_shared<ast::String>("1"),
            std::make_shared<ast::String>("1")
        ),
        /// If body
        std::make_shared<ast::Block>(std::vector<std::shared_ptr<ast::Object>>{
            std::make_shared<ast::String>("1")
        }),
        /// Else body
        std::make_shared<ast::Block>(std::vector<std::shared_ptr<ast::Object>>{
            std::make_shared<ast::String>("1")
        })
    );

    assert(if_block->else_body());
}

void parse_object_for_test()
{
    auto for_statement = std::make_shared<ast::For>();

    for_statement->set_body(std::make_shared<ast::Block>(std::vector<std::shared_ptr<ast::Object>>{}));

    for_statement->set_init(
        std::make_shared<ast::Binary>(
            lexeme_t::assign,
            std::make_shared<ast::Symbol>("var"),
            std::make_shared<ast::Number>(1)));

    for_statement->set_exit_condition(
        std::make_shared<ast::Binary>(
            lexeme_t::eq,
            std::make_shared<ast::Number>(1),
            std::make_shared<ast::Number>(1)));

    for_statement->set_increment(
        std::make_shared<ast::Unary>(
            lexeme_t::inc,
            std::make_shared<ast::Symbol>("var")));

    assert(std::dynamic_pointer_cast<ast::Binary>(for_statement->loop_init()));
    assert(std::dynamic_pointer_cast<ast::Binary>(for_statement->exit_condition()));
    assert(std::dynamic_pointer_cast<ast::Unary>(for_statement->increment()));
}

void parse_object_function_test()
{
    std::vector<std::shared_ptr<ast::Object>> arguments = {
        std::make_shared<ast::Number>("1"),
        std::make_shared<ast::Symbol>("Symbol")
    };

    std::vector<std::shared_ptr<ast::Object>> block_body = {
        std::make_shared<ast::Binary>(
            lexeme_t::plus,
            std::make_shared<ast::Number>("1"),
            std::make_shared<ast::Number>("1")
        )
    };

    auto function_object = std::make_shared<ast::Function>(
        "Name",
        arguments,
        std::make_shared<ast::Block>(std::move(block_body))
    );

    assert(function_object->name() == "Name");
    assert(function_object->arguments().size() == 2);
    assert(function_object->body()->same_with(
        std::make_shared<ast::Block>(
            std::vector<std::shared_ptr<ast::Object>>{
            std::make_shared<ast::Binary>(
                lexeme_t::plus,
                std::make_shared<ast::Number>("1"),
                std::make_shared<ast::Number>("1")
            )
        })
    ));
}

void parse_object_function_call_test()
{
    std::vector<std::shared_ptr<ast::Object>> arguments = {
        std::make_shared<ast::Number>("1"),
        std::make_shared<ast::Symbol>("Symbol")
    };

    auto function_call_object = std::make_shared<ast::FunctionCall>("Name", arguments);

    assert(function_call_object->name() == "Name");
    assert(function_call_object->arguments().size() == 2);
    assert(std::dynamic_pointer_cast<ast::Number>(function_call_object->arguments()[0])->value() == 1);
    assert(std::dynamic_pointer_cast<ast::Symbol>(function_call_object->arguments()[1])->name() == "Symbol");
}
#endif // AST_DEBUG

void run_expression_tests()
{
#ifdef AST_DEBUG
    std::cout << "Running parse tree tests...\n====\n";

    parse_object_number_test();
    parse_object_number_fuzz_test();
    parse_object_string_test();
    parse_object_array_test();
    parse_object_binary_operation_test();
    parse_object_unary_operation_test();
    parse_object_nested_expression_test();
    parse_object_while_tests();
    parse_object_if_tests();
    parse_object_if_else_tests();
    parse_object_for_test();
    parse_object_function_test();
    parse_object_function_call_test();
    parse_object_deep_nested_expression_test(10);
    parse_object_deep_nested_expression_test(100);
    parse_object_deep_nested_expression_test(1000);

    std::cout << "Parse tree tests passed successfully\n";
#endif // AST_DEBUG
}

#endif // PARSE_OBJECT_TESTS_HPP
