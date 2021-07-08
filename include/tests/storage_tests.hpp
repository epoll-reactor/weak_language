#ifndef SYMBOL_TABLE_TESTS_HPP
#define SYMBOL_TABLE_TESTS_HPP

#include "../storage/storage.hpp"

void test_found(const Storage& env, std::string_view name, bool expected_found_result)
{
    try
    {
        env.lookup(name.data());

    } catch (SemanticError&) {

        if (expected_found_result)
        {
            [[maybe_unused]] const bool variable_expected = false;

            assert(variable_expected);
        }
        else {
            return;
        }
    }

    if (!expected_found_result)
    {
        [[maybe_unused]] const bool not_found_expected = false;

        assert(not_found_expected);
    }
}

void symbol_table_basic_test()
{
    Storage env;

    env.push("var1", std::make_shared<ast::Symbol>("1"));
    env.push("var2", std::make_shared<ast::Symbol>("2"));

    test_found(env, "var1", true);
    test_found(env, "var2", true);
}

void symbol_table_flat_test()
{
    Storage env;

    env.push("var1", std::make_shared<ast::Symbol>("1"));
    env.push("var2", std::make_shared<ast::Symbol>("2"));

    env.scope_begin();

        env.push("var3", std::make_shared<ast::Symbol>("3"));

        test_found(env, "var1", true);
        test_found(env, "var2", true);
        test_found(env, "var3", true);

    env.scope_end();

    test_found(env, "var1", true);
    test_found(env, "var2", true);
    test_found(env, "var3", false);
}

void symbol_table_nested_test()
{
    Storage env;

    env.push("var1", std::make_shared<ast::Symbol>("1"));

    env.scope_begin();

        env.push("var2", std::make_shared<ast::Symbol>("2"));

        test_found(env, "var1", true);
        test_found(env, "var2", true);

        env.scope_begin();

            env.push("var3", std::make_shared<ast::Symbol>("3"));

            test_found(env, "var1", true);
            test_found(env, "var2", true);
            test_found(env, "var3", true);

        env.scope_end();

        test_found(env, "var1", true);
        test_found(env, "var2", true);
        test_found(env, "var3", false);

    env.scope_end();

    test_found(env, "var1", true);
    test_found(env, "var2", false);
    test_found(env, "var3", false);
}

void run_storage_tests()
{
    std::cout << "Running symbol table tests...\n====\n";

    symbol_table_basic_test();
    symbol_table_flat_test();
    symbol_table_nested_test();

    std::cout << "Symbol table tests passed successfully\n";
}

#endif // SYMBOL_TABLE_TESTS_HPP
