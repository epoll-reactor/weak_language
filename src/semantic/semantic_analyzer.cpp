#include "../../include/semantic/semantic_analyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer(boost::local_shared_ptr<ast::RootObject> input) noexcept(false) {
  for (const auto& expr : input->get()) {
    input_.emplace_back(expr);
  }
}

void SemanticAnalyzer::analyze() noexcept(false) {
  for (const auto& expression : input_) {
    analyze_statement(expression);
  }
}
void SemanticAnalyzer::analyze_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false) {
  // clang-format off
  switch (statement->ast_type()) {
    case ast::type_t::STRING:
    case ast::type_t::INTEGER:
    case ast::type_t::FLOAT:
    case ast::type_t::SYMBOL:
    case ast::type_t::TYPE_DEFINITION:
    case ast::type_t::TYPE_FIELD: {
      return;
    }
    case ast::type_t::BINARY: {
      const auto binary = boost::static_pointer_cast<ast::Binary>(statement);
      const token_t binary_type = binary->type();
      if (binary_type == token_t::assign || token_traits::is_assign_operator(binary_type)) {
        analyze_assign_statement(binary);
      } else {
        analyze_binary_statement(binary);
      }
      return;
    }
    case ast::type_t::UNARY: {
      const auto unary = boost::static_pointer_cast<ast::Unary>(statement);
      if (unary->operand()->ast_type() != ast::type_t::INTEGER && unary->operand()->ast_type() != ast::type_t::FLOAT && unary->operand()->ast_type() != ast::type_t::SYMBOL) {
        throw SemanticError("Invalid unary operands");
      }
      return;
    }
    case ast::type_t::BLOCK: {
      analyze_block_statement(boost::static_pointer_cast<ast::Block>(statement));
      return;
    }
    case ast::type_t::ARRAY: {
      analyze_array_statement(boost::static_pointer_cast<ast::Array>(statement));
      return;
    }
    case ast::type_t::LAMBDA: {
      analyze_lambda_statement(boost::static_pointer_cast<ast::Lambda>(statement));
      return;
    }
    case ast::type_t::LAMBDA_CALL: {
      analyze_lambda_call_statement(boost::static_pointer_cast<ast::LambdaCall>(statement));
      return;
    }
    case ast::type_t::IF: {
      analyze_if_statement(boost::static_pointer_cast<ast::If>(statement));
      return;
    }
    case ast::type_t::WHILE: {
      analyze_while_statement(boost::static_pointer_cast<ast::While>(statement));
      return;
    }
    case ast::type_t::FOR: {
      analyze_for_statement(boost::static_pointer_cast<ast::For>(statement));
      return;
    }
    default: { throw SemanticError("Unexpected statement"); }
  }
  // clang-format on
}

void SemanticAnalyzer::analyze_array_statement(const boost::local_shared_ptr<ast::Array>& statement) noexcept(false) {
  for (const auto& element : statement->elements()) {
    ast::type_t element_type = element->ast_type();
    if (element_type != ast::type_t::SYMBOL && element_type != ast::type_t::INTEGER && element_type != ast::type_t::FLOAT && element_type != ast::type_t::STRING && element_type != ast::type_t::BINARY && element_type != ast::type_t::LAMBDA_CALL && element_type != ast::type_t::ARRAY) {
      throw SemanticError("Array expects object don't statement");
    }
  }
}

void SemanticAnalyzer::analyze_assign_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false) {
  if (statement->lhs()->ast_type() != ast::type_t::SYMBOL) {
    throw SemanticError("Expression is not assignable");
  }

  const ast::type_t statement_type = statement->ast_type();
  if (statement_type != ast::type_t::SYMBOL && statement_type != ast::type_t::INTEGER && statement_type != ast::type_t::FLOAT && statement_type != ast::type_t::STRING && statement_type != ast::type_t::BINARY && statement_type != ast::type_t::UNARY && statement_type != ast::type_t::LAMBDA_CALL && statement_type != ast::type_t::ARRAY) {
    throw SemanticError("Expression is not assignable");
  }

  if (statement->rhs()->ast_type() == ast::type_t::BINARY) {
    analyze_statement(boost::static_pointer_cast<ast::Binary>(statement->rhs()));
  }
}

void SemanticAnalyzer::analyze_binary_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false) {
  if (!token_traits::is_binary(statement->type())) {
    throw SemanticError("Incorrect binary expression operator: {}", dispatch_token(statement->type()));
  }

  if (auto lhs = boost::dynamic_pointer_cast<ast::Binary>(statement->lhs())) {
    analyze_binary_statement(lhs);
  } else if (auto rhs = boost::dynamic_pointer_cast<ast::Binary>(statement->rhs())) {
    analyze_binary_statement(rhs);
  }
}

void SemanticAnalyzer::analyze_lambda_call_statement(const boost::local_shared_ptr<ast::LambdaCall>& lambda) noexcept(false) {
  for (const auto& argument : lambda->arguments()) {
    switch (argument->ast_type()) {
      case ast::type_t::INTEGER:
      case ast::type_t::FLOAT:
      case ast::type_t::STRING:
      case ast::type_t::SYMBOL:
      case ast::type_t::BINARY:
      case ast::type_t::UNARY:
      case ast::type_t::LAMBDA_CALL:
      case ast::type_t::TYPE_FIELD:
      case ast::type_t::ARRAY: {
        return;
      }
      default: {
        throw SemanticError("Wrong lambda call argument");
      }
    }
  }
}

void SemanticAnalyzer::analyze_lambda_statement(const boost::local_shared_ptr<ast::Lambda>& lambda) noexcept(false) {
  /// Lambda arguments are easily checked in parser.
  analyze_block_statement(lambda->body());
}

void SemanticAnalyzer::analyze_if_statement(const boost::local_shared_ptr<ast::If>& if_statement) noexcept(false) {
  if (!to_integral_convertible(if_statement->condition())) {
    throw SemanticError("If condition requires convertible to bool expression");
  }

  auto if_body = boost::dynamic_pointer_cast<ast::Block>(if_statement->body());
  auto else_body = boost::dynamic_pointer_cast<ast::Block>(if_statement->else_body());

  for (const auto& if_instruction : if_body->statements()) {
    analyze_statement(if_instruction);
  }

  if (else_body) {
    for (const auto& if_instruction : else_body->statements()) {
      analyze_statement(if_instruction);
    }
  }
}

void SemanticAnalyzer::analyze_while_statement(const boost::local_shared_ptr<ast::While>& while_statement) noexcept(false) {
  if (!to_number_convertible(while_statement->exit_condition())) {
    throw SemanticError("While condition requires convertible to bool expression");
  }

  auto body = boost::dynamic_pointer_cast<ast::Block>(while_statement->body());

  for (const auto& while_instruction : body->statements()) {
    analyze_statement(while_instruction);
  }
}

void SemanticAnalyzer::analyze_for_statement(const boost::local_shared_ptr<ast::For>& statement) noexcept(false) {
  if (statement->loop_init()) {
    if (statement->loop_init()->ast_type() != ast::type_t::BINARY) {
      throw SemanticError("Bad for init");
    }

    auto init = boost::static_pointer_cast<ast::Binary>(statement->loop_init());
    if (init->type() != token_t::assign) {
      throw SemanticError("For init part requires assignment operation");
    }
  }
  if (statement->exit_condition()) {
    if (!to_integral_convertible(statement->exit_condition())) {
      throw SemanticError("For condition requires convertible to bool expression");
    }
  }
  if (statement->increment()) {
    if (statement->increment()->ast_type() != ast::type_t::UNARY && statement->increment()->ast_type() != ast::type_t::BINARY) {
      throw SemanticError("Bad for increment part");
    }
  }
  for (const auto& instruction : statement->body()->statements()) {
    analyze_statement(instruction);
  }
}

void SemanticAnalyzer::analyze_block_statement(const boost::local_shared_ptr<ast::Block>& statement) noexcept(false) {
  for (const auto& instruction : statement->statements()) {
    analyze_statement(instruction);
  }
}

bool SemanticAnalyzer::to_integral_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false) {
  // clang-format off
  switch (statement->ast_type()) {
    case ast::type_t::SYMBOL:
    case ast::type_t::INTEGER:
    case ast::type_t::LAMBDA_CALL:
    case ast::type_t::UNARY: {
      return true;
    }
    case ast::type_t::BINARY: {
      auto binary = boost::static_pointer_cast<ast::Binary>(statement);
      try {
        analyze_binary_statement(binary);
      } catch (SemanticError& err) {
        return false;
      }
      return true;
    }
    default: { return false; }
  }
  // clang-format on
}

bool SemanticAnalyzer::to_number_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false) {
  return to_integral_convertible(statement) || statement->ast_type() == ast::type_t::FLOAT;
}
