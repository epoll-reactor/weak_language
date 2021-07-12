#ifndef EVAL_HPP
#define EVAL_HPP

#include <boost/pool/pool_alloc.hpp>

#include "../ast/ast.hpp"
#include "../storage/storage.hpp"

class Evaluator
{
public:
    Evaluator(const std::shared_ptr<ast::RootObject>& program);

    void eval();

private:
    boost::intrusive_ptr<ast::Object> call_function(std::string_view name, const std::vector<boost::intrusive_ptr<ast::Object>>& evaluated_args);

    boost::intrusive_ptr<ast::Object> eval_function_call(const boost::intrusive_ptr<ast::FunctionCall>& function_call);

    void eval_block(const boost::intrusive_ptr<ast::Block>& block);

    boost::intrusive_ptr<ast::Object> eval_binary(const boost::intrusive_ptr<ast::Binary>& binary);

    boost::intrusive_ptr<ast::Object> eval_unary(const boost::intrusive_ptr<ast::Unary>& unary);

    boost::intrusive_ptr<ast::Object> eval_array_subscript(const boost::intrusive_ptr<ast::ArraySubscriptOperator>& argument);

    void eval_array(const boost::intrusive_ptr<ast::Array>& array);

    void eval_for(const boost::intrusive_ptr<ast::For>& for_stmt);

    void eval_while(const boost::intrusive_ptr<ast::While>& while_stmt);

    void eval_if(const boost::intrusive_ptr<ast::If>& if_stmt);

    boost::intrusive_ptr<ast::Object> eval_expression(const boost::intrusive_ptr<ast::Object>& expression);

    template <typename T, typename... Args>
    T* pool_allocate(Args&&... args)
    {
        return new T(std::forward<Args>(args)...);
    }

    std::vector<boost::intrusive_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // EVAL_HPP
