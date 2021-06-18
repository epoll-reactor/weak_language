#ifndef SYMBOL_TABLE_TESTS_HPP
#define SYMBOL_TABLE_TESTS_HPP

#include "../semantic/symbol_table.hpp"

void symbol_table_flat_test()
{
    SymbolTable table;

    table.insert("variable1", symbol_context_t::variable);

    assert(table.lookup("variable1").name == "variable1");
    assert(table.lookup("variable1").depth == 1);
}

void symbol_table_nested_test()
{
    SymbolTable table;
    SymbolTable inner_table;

    inner_table.insert("variable1", symbol_context_t::variable);
    table.insert(inner_table);

    assert(table.lookup("variable1").name == "variable1");
    assert(table.lookup("variable1").depth == 2);
}

void symbol_table_test_inner_not_found()
{
    SymbolTable table;
    SymbolTable inner_table;

    table.insert("variable1", symbol_context_t::variable);
    table.insert(inner_table);

    try
    {
        inner_table.lookup("variable1");

    } catch (SemanticError&) {
        return;
    }

    const bool error_expected = false;

    assert(error_expected);
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
