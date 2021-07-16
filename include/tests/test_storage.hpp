#ifndef WEAK_TESTS_STORAGE_HPP
#define WEAK_TESTS_STORAGE_HPP

#include "../ast/ast.hpp"
#include "../storage/storage.hpp"

#include <boost/smart_ptr/make_local_shared.hpp>

#include <iostream>

void test_found(const Storage& env, std::string_view name, bool expected_found_result)
{
    try {
        env.lookup(name.data());

    } catch (EvalError&) {

        if (expected_found_result)
        {
            [[maybe_unused]] const bool variable_expected = false;

            assert(variable_expected);
        }
        else {
            return;
        }
    }

    if (!expected_found_result) {
        assert(false && "Variable unexpectedly found");
    }
}

void symbol_table_basic_test()
{
    Storage env;

    env.push("var1", boost::make_local_shared<ast::Symbol>("1"));
    env.push("var2", boost::make_local_shared<ast::Symbol>("2"));

    test_found(env, "var1", true);
    test_found(env, "var2", true);
}

void symbol_table_flat_test()
{
    Storage env;

    env.push("var1", boost::make_local_shared<ast::Symbol>("1"));
    env.push("var2", boost::make_local_shared<ast::Symbol>("2"));

    env.scope_begin();

        env.push("var3", boost::make_local_shared<ast::Symbol>("3"));

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

    env.push("var1", boost::make_local_shared<ast::Symbol>("1"));

    env.scope_begin();

        env.push("var2", boost::make_local_shared<ast::Symbol>("2"));

        test_found(env, "var1", true);
        test_found(env, "var2", true);

        env.scope_begin();

            env.push("var3", boost::make_local_shared<ast::Symbol>("3"));

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

#endif // WEAK_TESTS_STORAGE_HPP
