#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "../parser/ast.hpp"
#include "../semantic/semantic_error.hpp"
#include "../semantic/storage.hpp"

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(std::shared_ptr<ast::RootObject> input);

    void analyze();

private:
    void analyze_statement(std::shared_ptr<ast::Object> statement);

    void analyze_assign_statement(std::shared_ptr<ast::Binary> statement);

    void analyze_binary_statement(std::shared_ptr<ast::Binary> statement);

    void analyze_if_statement(std::shared_ptr<ast::If> if_statement);

    void analyze_while_statement(std::shared_ptr<ast::While> while_statement);

    void analyze_for_statement(std::shared_ptr<ast::For> for_statement);

    void analyze_function_call_statement(std::shared_ptr<ast::FunctionCall> function_statement);

    void analyze_function_statement(std::shared_ptr<ast::Function> function_statement);

    void analyze_block_statement(std::shared_ptr<ast::Block> block_statement);

    bool to_bool_convertible(std::shared_ptr<ast::Object> statement);

    std::vector<std::shared_ptr<ast::Object>> m_input;
};

#endif // SEMANTIC_ANALYZER_HPP