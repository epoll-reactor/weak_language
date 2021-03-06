#ifndef WEAK_EVAL_HPP
#define WEAK_EVAL_HPP

#include "../ast/ast.hpp"
#include "../storage/storage.hpp"

#include <boost/pool/pool_alloc.hpp>

class Evaluator {
public:
  Evaluator(const boost::local_shared_ptr<ast::RootObject>& program) noexcept(false);

  void eval() noexcept(false);

private:
  /// @throws EvalError if lambda not found
  /// @throws TypeError if non-lambdaal object passed
  /// @throws EvalError in case of mismatch in the number of arguments
  /// @throws all exceptions from eval
  boost::local_shared_ptr<ast::Object> call_lambda(std::string_view name, const std::vector<boost::local_shared_ptr<ast::Object>>& evaluated_args) noexcept(false);

  /// @throws all exceptions from call_lambda or builtin lambdas
  boost::local_shared_ptr<ast::Object> eval_lambda_call(const boost::local_shared_ptr<ast::LambdaCall>& lambda_call) noexcept(false);

  /// @throws all exceptions from eval
  void eval_block(const boost::local_shared_ptr<ast::Block>& block) noexcept(false);

  /// @throws EvalError from implementation in case of wrong binary operator
  /// @throws all exceptions from eval
  boost::local_shared_ptr<ast::Object> eval_binary(const boost::local_shared_ptr<ast::Binary>& binary) noexcept(false);

  /// @throws EvalError if operand variable not found
  /// @throws EvalError from implementation in case of wrong binary operator
  /// @throws all exceptions from eval
  boost::local_shared_ptr<ast::Object> eval_unary(const boost::local_shared_ptr<ast::Unary>& unary) noexcept(false);

  /// @throws all exceptions from eval
  void eval_array(const boost::local_shared_ptr<ast::Array>& array) noexcept(false);

  /// @throws all exceptions from eval
  void eval_for(const boost::local_shared_ptr<ast::For>& for_stmt) noexcept(false);

  /// @throws all exceptions from eval
  void eval_while(const boost::local_shared_ptr<ast::While>& while_stmt) noexcept(false);

  /// @throws all exceptions from eval
  void eval_if(const boost::local_shared_ptr<ast::If>& if_stmt) noexcept(false);

  void add_type_definition(const boost::local_shared_ptr<ast::TypeDefinition>& definition) noexcept(false);

  /// @throws std::out_of_range if no data is present
  boost::local_shared_ptr<ast::Object> eval_type_creation(const boost::local_shared_ptr<ast::TypeCreator>& type_creator) noexcept(false);

  /// @throws EvalError if type mismatch
  boost::local_shared_ptr<ast::Object> eval_type_field_access(const boost::local_shared_ptr<ast::TypeFieldOperator>& type_field) noexcept(false);

  /// @throws all exceptions from internal lambdas
  boost::local_shared_ptr<ast::Object> eval(const boost::local_shared_ptr<ast::Object>& expression) noexcept(false);

  std::vector<boost::local_shared_ptr<ast::Object>> expressions_;
  std::unordered_map<std::string, std::function<boost::local_shared_ptr<ast::Object>(const std::vector<boost::local_shared_ptr<ast::Object>>&)>> type_creators_;
  Storage storage_;
};

#endif// WEAK_EVAL_HPP
