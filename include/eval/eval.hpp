#ifndef EVAL_HPP
#define EVAL_HPP

#include "../parser/ast.hpp"
#include "../semantic/storage.hpp"


class Evaluator
{
public:
    Evaluator(const std::shared_ptr<ast::RootObject>& program);

    void eval();

private:
    std::shared_ptr<ast::Object> call_function(std::string_view name, const std::vector<std::shared_ptr<ast::Object>>& evaluated_args);

    std::shared_ptr<ast::Object> eval_function_call(const std::shared_ptr<ast::FunctionCall>& function_call);

    std::shared_ptr<ast::Object> eval_block(const std::shared_ptr<ast::Block>& block);

    std::shared_ptr<ast::Object> eval_binary(const std::shared_ptr<ast::Binary>& binary);

    std::shared_ptr<ast::Object> eval_array(const std::shared_ptr<ast::Array>& array);

    std::shared_ptr<ast::Object> eval_array_subscript(const std::shared_ptr<ast::ArraySubscriptOperator>& argument);

    void eval_while(const std::shared_ptr<ast::While>& while_stmt);

    void eval_if(const std::shared_ptr<ast::If>& if_stmt);

    void eval_for(const std::shared_ptr<ast::For>& for_stmt);

    std::shared_ptr<ast::Object> eval_expression(const std::shared_ptr<ast::Object>& expression);

    std::vector<std::shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // EVAL_HPP
