#ifndef WEAK_EVAL_HPP
#define WEAK_EVAL_HPP

#include "../ast/ast.hpp"
#include "../storage/storage.hpp"

#include <boost/pool/pool_alloc.hpp>

class Evaluator
{
public:
    Evaluator(const boost::local_shared_ptr<ast::RootObject>& program);

    void eval() noexcept(false);

private:
    /// @throws EvalError if function not found
    /// @throws TypeError if non-functional object passed
    /// @throws EvalError in case of mismatch in the number of arguments
    /// @throws all exceptions from eval
    boost::local_shared_ptr<ast::Object> call_function(std::string_view name, std::vector<boost::local_shared_ptr<ast::Object>> evaluated_args) noexcept(false);

    /// @throws all exceptions from call_function or builtin functions
    boost::local_shared_ptr<ast::Object> eval_function_call(const boost::local_shared_ptr<ast::FunctionCall>& function_call) noexcept(false);

    /// @throws all exceptions from eval
    void eval_block(const boost::local_shared_ptr<ast::Block>& block) noexcept(false);

    /// @throws EvalError from implementation in case of wrong binary operator
    /// @throws all exceptions from eval
    boost::local_shared_ptr<ast::Object> eval_binary(const boost::local_shared_ptr<ast::Binary>& binary) noexcept(false);

    /// @throws EvalError if operand variable not found
    /// @throws EvalError from implementation in case of wrong binary operator
    /// @throws all exceptions from eval
    boost::local_shared_ptr<ast::Object> eval_unary(const boost::local_shared_ptr<ast::Unary>& unary) noexcept(false);

    /// @throws EvalError if function not found
    /// @throws EvalError if non-subscript object passed
    /// @throws EvalError if out-of-range
    /// @throws all exceptions from eval
    boost::local_shared_ptr<ast::Object> eval_array_subscript(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& argument) noexcept(false);

    /// @throws all exceptions from eval
    void eval_array(const boost::local_shared_ptr<ast::Array>& array) noexcept(false);

    /// @throws all exceptions from eval
    void eval_for(const boost::local_shared_ptr<ast::For>& for_stmt) noexcept(false);

    /// @throws all exceptions from eval
    void eval_while(const boost::local_shared_ptr<ast::While>& while_stmt) noexcept(false);

    /// @throws all exceptions from eval
    void eval_if(const boost::local_shared_ptr<ast::If>& if_stmt) noexcept(false);

    /// @throws all exceptions from internal functions
    boost::local_shared_ptr<ast::Object> eval(const boost::local_shared_ptr<ast::Object>& expression) noexcept(false);

    std::vector<boost::local_shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // WEAK_EVAL_HPP
