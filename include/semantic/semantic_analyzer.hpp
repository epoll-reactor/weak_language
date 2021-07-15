#ifndef WEAK_SEMANTIC_ANALYZER_HPP
#define WEAK_SEMANTIC_ANALYZER_HPP

#include "../ast/ast.hpp"
#include "../error/semantic_error.hpp"
#include "../storage/storage.hpp"

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(boost::local_shared_ptr<ast::RootObject> input) noexcept(false);

    /// @throws SemanticError while analyzing
    void analyze() noexcept(false);

private:
    void analyze_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false);

    void analyze_array_statement(const boost::local_shared_ptr<ast::Array>& statement) noexcept(false);

    void analyze_assign_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false);

    void analyze_binary_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false);

    void analyze_array_subscript_statement(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& statement) noexcept(false);

    void analyze_function_call_statement(const boost::local_shared_ptr<ast::FunctionCall>& function_statement) noexcept(false);

    void analyze_function_statement(const boost::local_shared_ptr<ast::Function>& function_statement) noexcept(false);

    void analyze_if_statement(const boost::local_shared_ptr<ast::If>& if_statement) noexcept(false);

    void analyze_while_statement(const boost::local_shared_ptr<ast::While>& while_statement) noexcept(false);

    void analyze_for_statement(const boost::local_shared_ptr<ast::For>& for_statement) noexcept(false);

    void analyze_block_statement(const boost::local_shared_ptr<ast::Block>& block_statement) noexcept(false);

    bool to_integral_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false);

    bool to_number_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false);

    std::vector<boost::local_shared_ptr<ast::Object>> m_input;
};

#endif // WEAK_SEMANTIC_ANALYZER_HPP
