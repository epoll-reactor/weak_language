#include "../../include/parser/parser.hpp"

Parser::Parser(std::vector<Token> tokens) noexcept(true)
  : input_(std::move(tokens))
  , current_index_(0) {
}

boost::local_shared_ptr<ast::RootObject> Parser::parse() noexcept(false) {
  boost::local_shared_ptr<ast::RootObject> root = boost::make_local_shared<ast::RootObject>();
  while (has_next()) {
    if (current().type == token_t::LEFT_BRACE) {
      root->add(block());
    } else {
      auto expression = additive();
      if (!is_block_statement(expression)) {
        require({token_t::SEMICOLON});
      }
      root->add(std::move(expression));
    }
  }
  return root;
}

boost::local_shared_ptr<ast::Object> Parser::primary() noexcept(false) {
  peek();
  // clang-format off
  switch (previous().type) {
    case token_t::IF: { return if_statement(); }
    case token_t::WHILE: { return while_statement(); }
    case token_t::FOR: { return for_statement(); }
    case token_t::LAMBDA: { return lambda_declare_statement(); }
    case token_t::DEFINE_TYPE: { return define_type_statement(); }
    case token_t::NEW: { return type_creator(); }
    case token_t::LEFT_BRACE: { return block(); }
    case token_t::LEFT_BOX_BRACE: { return array(); }
    case token_t::LEFT_PAREN: { return resolve_braced_expression(); }
    case token_t::NUM: { return binary(boost::make_local_shared<ast::Integer>(previous().data)); }
    case token_t::FLOAT: { return binary(boost::make_local_shared<ast::Float>(previous().data)); }
    case token_t::STRING_LITERAL: { return boost::make_local_shared<ast::String>(previous().data); }
    case token_t::SYMBOL: { return resolve_symbol(); }
    case token_t::MINUS:
    case token_t::NEGATION:
    case token_t::INC:
    case token_t::DEC: {
      return unary();
    }

    default: {
      throw ParseError("Unknown expression: {}", dispatch_token(previous().type));
    }
  }
  // clang-format off
}

const Token& Parser::current() const noexcept(false) {
  return input_.at(current_index_);
}

const Token& Parser::previous() const noexcept(false) {
  return input_.at(current_index_ - 1);
}

const Token& Parser::peek() noexcept(false) {
  return input_.at(current_index_++);
}

bool Parser::end_of_expression() const noexcept(true) {
  switch (current().type) {
    case token_t::SEMICOLON:
    case token_t::RIGHT_PAREN:
    case token_t::COMMA:
    case token_t::RIGHT_BOX_BRACE: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool Parser::is_block_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true) {
  switch (statement->ast_type()) {
    case ast::type_t::IF:
    case ast::type_t::WHILE:
    case ast::type_t::LAMBDA:
    case ast::type_t::FOR: {
      return true;
    }
    default: {
      return false;
    }
  }
}

bool Parser::has_next() const noexcept(false) {
  return current_index_ < input_.size() && current().type != token_t::END_OF_DATA;
}

bool Parser::is_block(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true) {
  return statement->ast_type() == ast::type_t::BLOCK;
}

std::optional<Token> Parser::match(const std::vector<token_t>& expected_types) noexcept(false) {
  if (has_next()) {
    const Token token = current();
    if (std::find(expected_types.begin(), expected_types.end(), token.type) != expected_types.end()) {
      peek();
      return token;
    }
  }
  return std::nullopt;
}

Token Parser::require(const std::vector<token_t>& expected_types) noexcept(false) {
  if (const std::optional<Token> token = match(expected_types)) {
    return token.value();
  } else {
    throw ParseError("{} expected, got {}", dispatch_token(expected_types[0]), dispatch_token(current().type));
  }
}

boost::local_shared_ptr<ast::Object> Parser::additive() noexcept(false) {
  auto expr = multiplicative();
  while (true) {
    if (previous().type == token_t::PLUS) {
      expr = boost::make_local_shared<ast::Binary>(token_t::PLUS, expr, multiplicative());
      continue;
    }
    if (previous().type == token_t::MINUS) {
      expr = boost::make_local_shared<ast::Binary>(token_t::MINUS, expr, multiplicative());
      continue;
    }
    break;
  }
  return expr;
}

boost::local_shared_ptr<ast::Object> Parser::multiplicative() noexcept(false) {
  auto expr = primary();
  while (true) {
    if (previous().type == token_t::STAR) {
      expr = boost::make_local_shared<ast::Binary>(token_t::STAR, expr, multiplicative());
      continue;
    }
    if (previous().type == token_t::SLASH) {
      expr = boost::make_local_shared<ast::Binary>(token_t::SLASH, expr, multiplicative());
      continue;
    }
    if (previous().type == token_t::MOD) {
      expr = boost::make_local_shared<ast::Binary>(token_t::MOD, expr, multiplicative());
      continue;
    }
    break;
  }
  return expr;
}

boost::local_shared_ptr<ast::Object> Parser::binary(const boost::local_shared_ptr<ast::Object>& ptr) noexcept(false) {
  if (end_of_expression()) {
    return ptr;
  }
  const token_t op = peek().type;
  return boost::make_local_shared<ast::Binary>(op, ptr, additive());
}

boost::local_shared_ptr<ast::Object> Parser::unary() noexcept(false) {
  const token_t op = previous().type;
  return boost::make_local_shared<ast::Unary>(op, primary());
}

boost::local_shared_ptr<ast::Block> Parser::block() noexcept(false) {
  require({token_t::LEFT_BRACE});
  std::vector<boost::local_shared_ptr<ast::Object>> stmts;
  while (current().type != token_t::RIGHT_BRACE) {
    const auto stmt = additive();
    stmts.push_back(stmt);
    if (is_block(stmt)) {
      require({token_t::RIGHT_BRACE});
    } else if (!is_block_statement(stmt)) {
      require({token_t::SEMICOLON});
    }
  }
  require({token_t::RIGHT_BRACE});
  return boost::make_local_shared<ast::Block>(std::move(stmts));
}

boost::local_shared_ptr<ast::Object> Parser::array() noexcept(false) {
  std::vector<boost::local_shared_ptr<ast::Object>> objects;
  while (true) {
    if (current().type == token_t::RIGHT_BOX_BRACE) {
      require({token_t::RIGHT_BOX_BRACE});
      break;
    }
    objects.emplace_back(primary());
    const auto term = require({token_t::RIGHT_BOX_BRACE, token_t::COMMA});
    if (term.type == token_t::COMMA) {
      continue;
    }
    if (term.type == token_t::RIGHT_BOX_BRACE) {
      break;
    }
  }
  return boost::make_local_shared<ast::Array>(std::move(objects));
}

boost::local_shared_ptr<ast::Object> Parser::if_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  auto if_condition = primary();
  require({token_t::RIGHT_PAREN});
  auto if_body = block();
  if (match({token_t::ELSE})) {
    auto else_body = block();
    return boost::make_local_shared<ast::If>(std::move(if_condition), std::move(if_body), std::move(else_body));
  } else {
    return boost::make_local_shared<ast::If>(std::move(if_condition), std::move(if_body));
  }
}

boost::local_shared_ptr<ast::Object> Parser::while_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  auto while_condition = primary();
  require({token_t::RIGHT_PAREN});
  auto while_body = block();
  return boost::make_local_shared<ast::While>(std::move(while_condition), std::move(while_body));
}

boost::local_shared_ptr<ast::Object> Parser::for_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  boost::local_shared_ptr<ast::Object> for_init;
  boost::local_shared_ptr<ast::Object> for_exit_condition;
  boost::local_shared_ptr<ast::Object> for_increment;
  if (!match({token_t::SEMICOLON})) {
    for_init = primary();
    require({token_t::SEMICOLON});
  }
  if (!match({token_t::SEMICOLON})) {
    for_exit_condition = primary();
    require({token_t::SEMICOLON});
  }
  if (!match({token_t::RIGHT_PAREN})) {
    for_increment = primary();
    require({token_t::RIGHT_PAREN});
  }
  auto for_body = block();
  auto for_statement = boost::make_local_shared<ast::For>();
  if (for_init) {
    for_statement->set_init(std::move(for_init));
  }
  if (for_exit_condition) {
    for_statement->set_exit_condition(std::move(for_exit_condition));
  }
  if (for_increment) {
    for_statement->set_increment(std::move(for_increment));
  }
  for_statement->set_body(std::move(for_body));
  return for_statement;
};

boost::local_shared_ptr<ast::Object> Parser::lambda_declare_statement() noexcept(false) {
  const Token symbol = require({token_t::SYMBOL});
  const std::string lambda_name = symbol.data;
  require({token_t::LEFT_PAREN});
  std::vector<boost::local_shared_ptr<ast::Object>> arguments;
  if (!match({token_t::RIGHT_PAREN})) {
    while (true) {
      if (current().type != token_t::SYMBOL) {
        throw ParseError("Symbol as lambda parameter expected");
      } else {
        arguments.emplace_back(boost::make_local_shared<ast::Symbol>(current().data));
      }
      peek();
      const auto term = require({token_t::RIGHT_PAREN, token_t::COMMA});
      if (term.type == token_t::COMMA) {
        continue;
      } else if (term.type == token_t::RIGHT_PAREN) {
        break;
      }
    }
  }
  auto lambda_body = block();
  return boost::make_local_shared<ast::Lambda>(lambda_name, std::move(arguments), std::move(lambda_body));
}

boost::local_shared_ptr<ast::Object> Parser::define_type_statement() noexcept(false) {
  const std::string name = require({token_t::SYMBOL}).data;
  require({token_t::LEFT_PAREN});
  std::vector<std::string> fields;
  while (true) {
    if (current().type != token_t::SYMBOL) {
      throw ParseError("Symbol as type field expected");
    } else {
      fields.push_back(current().data);
    }
    peek();
    const auto term = require({token_t::RIGHT_PAREN, token_t::COMMA});
    if (term.type == token_t::COMMA) {
      continue;
    } else if (term.type == token_t::RIGHT_PAREN) {
      break;
    }
  }
  return boost::make_local_shared<ast::TypeDefinition>(name, std::move(fields));
}

std::vector<boost::local_shared_ptr<ast::Object>> Parser::resolve_lambda_arguments() noexcept(false) {
  require({token_t::LEFT_PAREN});
  if (match({token_t::RIGHT_PAREN})) {
    return {};
  }
  std::vector<boost::local_shared_ptr<ast::Object>> arguments;
  while (true) {
    arguments.push_back(primary());
    const auto term = require({token_t::RIGHT_PAREN, token_t::COMMA});
    if (term.type == token_t::COMMA) {
      continue;
    } else if (term.type == token_t::RIGHT_PAREN) {
      break;
    }
  };
  return arguments;
}

boost::local_shared_ptr<ast::Object> Parser::resolve_type_field_operator() noexcept(false) {
  std::string symbol_name = previous().data;
  require({token_t::DOT});
  peek();
  std::string field_name = previous().data;
  return boost::make_local_shared<ast::TypeFieldOperator>(std::move(symbol_name), std::move(field_name));
}

boost::local_shared_ptr<ast::Object> Parser::resolve_symbol() noexcept(false) {
  switch (current().type) {
    case token_t::LEFT_PAREN: {
      std::string data = previous().data;
      return boost::make_local_shared<ast::LambdaCall>(std::move(data), resolve_lambda_arguments());
    }
    case token_t::DOT: {
      return resolve_type_field_operator();
    }
    default: {
      return binary(boost::make_local_shared<ast::Symbol>(previous().data));
    }
  }
}

boost::local_shared_ptr<ast::Object> Parser::resolve_braced_expression() noexcept(false) {
  --current_index_;
  require({token_t::LEFT_PAREN});
  auto expr = primary();
  require({token_t::RIGHT_PAREN});
  return expr;
}

boost::local_shared_ptr<ast::Object> Parser::type_creator() noexcept(false) {
  std::string name = peek().data;
  if (current().type == token_t::LEFT_PAREN) {
    return boost::make_local_shared<ast::TypeCreator>(std::move(name), resolve_lambda_arguments());
  } else {
    throw ParseError("'(' expected");
  }
}
