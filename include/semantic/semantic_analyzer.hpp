#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "../ast/ast.hpp"
#include "../error/semantic_error.hpp"
#include "../storage/storage.hpp"

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(const std::shared_ptr<ast::RootObject>& input);

    void analyze();

private:
    void analyze_statement(const boost::intrusive_ptr<ast::Object>& statement);

    void analyze_array_statement(const boost::intrusive_ptr<ast::Array>& statement);

    void analyze_assign_statement(const boost::intrusive_ptr<ast::Binary>& statement);

    void analyze_binary_statement(const boost::intrusive_ptr<ast::Binary>& statement);

    void analyze_array_subscript_statement(const boost::intrusive_ptr<ast::ArraySubscriptOperator>& statement);

    void analyze_function_call_statement(const boost::intrusive_ptr<ast::FunctionCall>& function_statement);

    void analyze_function_statement(const boost::intrusive_ptr<ast::Function>& function_statement);

    void analyze_if_statement(const boost::intrusive_ptr<ast::If>& if_statement);

    void analyze_while_statement(const boost::intrusive_ptr<ast::While>& while_statement);

    void analyze_for_statement(const boost::intrusive_ptr<ast::For>& for_statement);

    void analyze_block_statement(const boost::intrusive_ptr<ast::Block>& block_statement);

    bool to_integral_convertible(const boost::intrusive_ptr<ast::Object>& statement);

    bool to_number_convertible(const boost::intrusive_ptr<ast::Object>& statement);

    std::vector<boost::intrusive_ptr<ast::Object>> m_input;
};

#endif // SEMANTIC_ANALYZER_HPP
