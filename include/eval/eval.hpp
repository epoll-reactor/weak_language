#ifndef EVAL_HPP
#define EVAL_HPP

#include "../parser/ast.hpp"
#include "../storage/storage.hpp"

#include <boost/pool/pool_alloc.hpp>

class Evaluator
{
public:
    Evaluator(const std::shared_ptr<ast::RootObject>& program);

    void eval();

private:
    std::shared_ptr<ast::Object> call_function(std::string_view name, const std::vector<std::shared_ptr<ast::Object>>& evaluated_args);

    std::shared_ptr<ast::Object> eval_function_call(const std::shared_ptr<ast::FunctionCall>& function_call);

    void eval_block(const std::shared_ptr<ast::Block>& block);

    std::shared_ptr<ast::Object> eval_binary(const std::shared_ptr<ast::Binary>& binary);

    std::shared_ptr<ast::Object> eval_array_subscript(const std::shared_ptr<ast::ArraySubscriptOperator>& argument);

    void eval_array(const std::shared_ptr<ast::Array>& array);

    void eval_for(const std::shared_ptr<ast::For>& for_stmt);

    void eval_while(const std::shared_ptr<ast::While>& while_stmt);

    void eval_if(const std::shared_ptr<ast::If>& if_stmt);

    std::shared_ptr<ast::Object> eval_expression(const std::shared_ptr<ast::Object>& expression);

    template <typename T, typename... Args>
    std::shared_ptr<T> pool_allocate(Args&&... args)
    {
        return std::allocate_shared<T, decltype(m_pool_allocator)>(m_pool_allocator, std::forward<Args>(args)...);
    }

    boost::pool_allocator<
        ast::Object,
        boost::default_user_allocator_new_delete,
        boost::details::pool::default_mutex,
        16,
        std::numeric_limits<int>::max()
    > m_pool_allocator;
    std::vector<std::shared_ptr<ast::Object>> m_expressions;
    Storage m_storage;
};

#endif // EVAL_HPP
