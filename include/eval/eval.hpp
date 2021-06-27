#ifndef EVAL_HPP
#define EVAL_HPP

#include "../parser/ast.hpp"
#include "../semantic/storage.hpp"


class Evaluator
{
public:
    Evaluator(std::shared_ptr<ast::RootObject> program);

    void eval();

private:
    std::shared_ptr<ast::Object> call_function(std::string_view name, std::vector<std::shared_ptr<ast::Object>> evaluated_args);

    std::shared_ptr<ast::Object> eval_function_call(std::shared_ptr<ast::FunctionCall> function_call);

    std::shared_ptr<ast::Object> eval_binary(std::shared_ptr<ast::Binary> binary);

    void eval_while(std::shared_ptr<ast::While> while_stmt);

    void eval_if(std::shared_ptr<ast::If> if_stmt);

    void eval_for(std::shared_ptr<ast::For> for_stmt);

    std::shared_ptr<ast::Object> eval_expression(std::shared_ptr<ast::Object> expression);

    std::vector<std::shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // EVAL_HPP
