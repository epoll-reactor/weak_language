#ifndef SYMBOL_TABLE_TESTS_HPP
#define SYMBOL_TABLE_TESTS_HPP

#include "../semantic/environment.hpp"

void symbol_table_flat_test()
{
    Environment env;

    env.push("var1", "1");
    env.push("var2", "2");

    env.scope_begin();

    env.push("var3", "3");

    assert(env.lookup("var1"));
    assert(env.lookup("var2"));
    assert(env.lookup("var3"));

    env.scope_end();

    assert(env.lookup("var1"));
    assert(env.lookup("var2"));
    assert(!env.lookup("var3"));
}

void symbol_table_nested_test()
{
    Environment env;

    env.push("var1", "1");

    env.scope_begin();

        env.push("var2", "2");

        assert(env.lookup("var1"));
        assert(env.lookup("var2"));

        env.scope_begin();

            env.push("var3", "3");

            assert(env.lookup("var1"));
            assert(env.lookup("var2"));
            assert(env.lookup("var3"));

        env.scope_end();

        assert( env.lookup("var1"));
        assert( env.lookup("var2"));
        assert(!env.lookup("var3"));

    env.scope_end();

    assert( env.lookup("var1"));
    assert(!env.lookup("var2"));
    assert(!env.lookup("var3"));
}

void symbol_table_test_inner_not_found()
{

}

void run_symbol_table_tests()
{
    std::cout << "Running symbol table tests...\n====\n";

    symbol_table_flat_test();
    symbol_table_nested_test();
    symbol_table_test_inner_not_found();

    std::cout << "Symbol table tests passed successfully\n";
}

#endif // SYMBOL_TABLE_TESTS_HPP
