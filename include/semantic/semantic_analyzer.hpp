#ifndef WEAK_SEMANTIC_ANALYZER_HPP
#define WEAK_SEMANTIC_ANALYZER_HPP

#include "../ast/ast.hpp"
#include "../error/semantic_error.hpp"
#include "../storage/storage.hpp"

class SemanticAnalyzer {
public:
  SemanticAnalyzer(boost::local_shared_ptr<ast::RootObject> input) noexcept(false);

  /// @throws SemanticError while analyzing
  void analyze() noexcept(false);

private:
  void analyze_statement(const boost::local_shared_ptr<ast::Object>&) noexcept(false);

  void analyze_array_statement(const boost::local_shared_ptr<ast::Array>&) noexcept(false);

  void analyze_assign_statement(const boost::local_shared_ptr<ast::Binary>&) noexcept(false);

  void analyze_binary_statement(const boost::local_shared_ptr<ast::Binary>&) noexcept(false);

  void analyze_lambda_call_statement(const boost::local_shared_ptr<ast::LambdaCall>&) noexcept(false);

  void analyze_lambda_statement(const boost::local_shared_ptr<ast::Lambda>&) noexcept(false);

  void analyze_if_statement(const boost::local_shared_ptr<ast::If>&) noexcept(false);

  void analyze_while_statement(const boost::local_shared_ptr<ast::While>&) noexcept(false);

  void analyze_for_statement(const boost::local_shared_ptr<ast::For>&) noexcept(false);

  void analyze_block_statement(const boost::local_shared_ptr<ast::Block>&) noexcept(false);

  bool to_integral_convertible(const boost::local_shared_ptr<ast::Object>&) noexcept(false);

  bool to_number_convertible(const boost::local_shared_ptr<ast::Object>&) noexcept(false);

  std::vector<boost::local_shared_ptr<ast::Object>> input_;
};

#endif// WEAK_SEMANTIC_ANALYZER_HPP
