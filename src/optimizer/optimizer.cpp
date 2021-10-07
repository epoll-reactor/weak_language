#include "../../include/optimizer/optimizer.hpp"

#include "../../include/ast/ast.hpp"
#include "../../include/eval/implementation/unary.hpp"

static void optimize(
    std::vector<boost::local_shared_ptr<ast::Object>>& outside_block,
    boost::local_shared_ptr<ast::Object>& expression,
    ssize_t to_erase) noexcept(false);

static void optimize_while(
    std::vector<boost::local_shared_ptr<ast::Object>>& outside_block,
    boost::local_shared_ptr<ast::Object>& expression,
    ssize_t to_erase) noexcept(false);

static void optimize_for(
    std::vector<boost::local_shared_ptr<ast::Object>>& outside_block,
    boost::local_shared_ptr<ast::Object>& expression,
    ssize_t to_erase) noexcept(false);

static void optimize_unary(
    std::vector<boost::local_shared_ptr<ast::Object>>& outside_block,
    boost::local_shared_ptr<ast::Object>& expression,
    ssize_t to_erase) noexcept(false);

static void optimize_binary(
    std::vector<boost::local_shared_ptr<ast::Object>>& outside_block,
    boost::local_shared_ptr<ast::Object>& expression,
    ssize_t to_erase) noexcept(false);

Optimizer::Optimizer(boost::local_shared_ptr<ast::RootObject>& root)
  : input_(root->get()) {}

void Optimizer::optimize() {
  for (auto& expr : input_) {
    if (expr->ast_type() != ast::ast_type_t::FUNCTION) {
      continue;
    }

    ssize_t to_erase = 0;
    auto function = boost::static_pointer_cast<ast::Function>(expr);
    auto& function_stmts = function->body()->statements();
    for (auto& function_expr : function_stmts) {
      ::optimize(function_stmts, function_expr, to_erase);
      ++to_erase;
    }
  }
}

void optimize(std::vector<boost::local_shared_ptr<ast::Object>>& outside_block, boost::local_shared_ptr<ast::Object>& expression, ssize_t to_erase) noexcept(false) {
  if (!expression) {
    return;
  }

  switch (expression->ast_type()) {
    case ast::ast_type_t::WHILE: optimize_while(outside_block, expression, to_erase); return;
    case ast::ast_type_t::FOR: optimize_for(outside_block, expression, to_erase); return;
    case ast::ast_type_t::UNARY: optimize_unary(outside_block, expression, to_erase); return;
    case ast::ast_type_t::BINARY: optimize_binary(outside_block, expression, to_erase); return;
    default: return;
  }
}

void optimize_while(std::vector<boost::local_shared_ptr<ast::Object>>& outside_block, boost::local_shared_ptr<ast::Object>& expression, ssize_t to_erase) noexcept(false) {
  auto while_expr = boost::static_pointer_cast<ast::While>(expression);
  auto& statements = while_expr->body()->statements();

  auto exit_condition_type = while_expr->exit_condition()->ast_type();

  if (exit_condition_type == ast::ast_type_t::INTEGER || exit_condition_type == ast::ast_type_t::FLOAT || exit_condition_type == ast::ast_type_t::SYMBOL) {
    if (statements.empty()) {
      outside_block.erase(outside_block.begin() + to_erase);
      return;
    }
  }

  for (auto& while_statement : statements) {
    optimize(outside_block, while_statement, to_erase);
  }
}

void optimize_for(std::vector<boost::local_shared_ptr<ast::Object>>& outside_block, boost::local_shared_ptr<ast::Object>& expression, ssize_t to_erase) noexcept(false) {
  auto for_expr = boost::static_pointer_cast<ast::For>(expression);
  auto& statements = for_expr->body()->statements();

  if (!for_expr->exit_condition()) {
    if (statements.empty()) {
      outside_block.erase(outside_block.begin() + to_erase);
      return;
    }
  }

  for (auto& for_statement : statements) {
    optimize(outside_block, for_statement, to_erase);
  }
}

void optimize_unary(std::vector<boost::local_shared_ptr<ast::Object>>& outside_block, boost::local_shared_ptr<ast::Object>& expression, ssize_t to_erase) noexcept(false) {
  auto unary = boost::static_pointer_cast<ast::Unary>(expression);

  if (unary->operand()->ast_type() == ast::ast_type_t::INTEGER || unary->operand()->ast_type() == ast::ast_type_t::FLOAT) {
    outside_block[to_erase] = internal::unary_implementation(
        unary->operand()->ast_type(),
        unary->type(),
        unary->operand());
  }
}

void optimize_binary(std::vector<boost::local_shared_ptr<ast::Object>>& outside_block, boost::local_shared_ptr<ast::Object>& expression, ssize_t to_erase) noexcept(false) {
  UNUSED(outside_block);
  UNUSED(to_erase);

  auto binary = boost::static_pointer_cast<ast::Binary>(expression);

  // ...
}