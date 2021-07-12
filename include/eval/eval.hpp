#ifndef WEAK_EVAL_HPP
#define WEAK_EVAL_HPP

#include <boost/pool/pool_alloc.hpp>

#include "../ast/ast.hpp"
#include "../storage/storage.hpp"

class Evaluator
{
public:
    Evaluator(const boost::local_shared_ptr<ast::RootObject>& program);

    void eval();

private:
    boost::local_shared_ptr<ast::Object> call_function(std::string_view name, const std::vector<boost::local_shared_ptr<ast::Object>>& evaluated_args);

    boost::local_shared_ptr<ast::Object> eval_function_call(const boost::local_shared_ptr<ast::FunctionCall>& function_call);

    void eval_block(const boost::local_shared_ptr<ast::Block>& block);

    boost::local_shared_ptr<ast::Object> eval_binary(const boost::local_shared_ptr<ast::Binary>& binary);

    boost::local_shared_ptr<ast::Object> eval_unary(const boost::local_shared_ptr<ast::Unary>& unary);

    boost::local_shared_ptr<ast::Object> eval_array_subscript(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& argument);

    void eval_array(const boost::local_shared_ptr<ast::Array>& array);

    void eval_for(const boost::local_shared_ptr<ast::For>& for_stmt);

    void eval_while(const boost::local_shared_ptr<ast::While>& while_stmt);

    void eval_if(const boost::local_shared_ptr<ast::If>& if_stmt);

    boost::local_shared_ptr<ast::Object> eval_expression(const boost::local_shared_ptr<ast::Object>& expression);

    template <typename T, typename... Args>
    T* pool_allocate(Args&&... args)
    {
        return new T(std::forward<Args>(args)...);
    }

    std::vector<boost::local_shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // WEAK_EVAL_HPP
