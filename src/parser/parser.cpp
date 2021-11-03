#include "../../include/parser/parser.hpp"

template <typename T, typename... Args>
static auto make_ast_ptr(Args&&... args) noexcept(false) {
  return boost::make_local_shared<T>(std::forward<Args>(args)...);
}

static bool is_block_statement(const Parser::ast_ptr<ast::Object>& statement) noexcept(true) {
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

static bool is_block(const Parser::ast_ptr<ast::Object>& statement) noexcept(true) {
  return statement->ast_type() == ast::type_t::BLOCK;
}

Parser::Parser(std::vector<Token> tokens) noexcept(true)
  : input_(std::move(tokens))
  , current_index_(0) {}

Parser::ast_ptr<ast::RootObject> Parser::parse() noexcept(false) {
  ast_ptr<ast::RootObject> root = make_ast_ptr<ast::RootObject>();
  while (has_next()) {
    auto expression = additive();
    if (!is_block_statement(expression)) {
      require({token_t::SEMICOLON});
    }
    root->add(std::move(expression));
  }
  return root;
}

Parser::ast_ptr<ast::Object> Parser::primary() noexcept(false) {
  peek();
  switch (previous().type) {
    case token_t::IF: {
      return if_statement();
    }
    case token_t::WHILE: {
      return while_statement();
    }
    case token_t::FOR: {
      return for_statement();
    }
    case token_t::LAMBDA: {
      return lambda_declare_statement();
    }
    case token_t::DEFINE_TYPE: {
      return define_type_statement();
    }
    case token_t::NEW: {
      return type_creator();
    }
    case token_t::LEFT_BRACE: {
      return block();
    }
    case token_t::LEFT_BOX_BRACE: {
      return array();
    }
    case token_t::LEFT_PAREN: {
      return resolve_braced_expression();
    }
    case token_t::NUM: {
      return binary(make_ast_ptr<ast::Integer>(previous().data));
    }
    case token_t::FLOAT: {
      return binary(make_ast_ptr<ast::Float>(previous().data));
    }
    case token_t::STRING_LITERAL: {
      return make_ast_ptr<ast::String>(previous().data);
    }
    case token_t::SYMBOL: {
      return resolve_symbol();
    }
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

bool Parser::has_next() const noexcept(false) {
  return current_index_ < input_.size() && current().type != token_t::END_OF_DATA;
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

Parser::ast_ptr<ast::Object> Parser::additive() noexcept(false) {
  auto expr = multiplicative();
  while (true) {
    const token_t op = previous().type;
    switch (op) {
      case token_t::PLUS:
      case token_t::MINUS: {
        expr = make_ast_ptr<ast::Binary>(op, expr, multiplicative());
        continue;
      }
      default: {
        break;
      }
    }
    break;
  }
  return expr;
}

Parser::ast_ptr<ast::Object> Parser::multiplicative() noexcept(false) {
  auto expr = primary();
  while (true) {
    const token_t op = previous().type;
    switch (op) {
      case token_t::STAR:
      case token_t::SLASH:
      case token_t::MOD: {
        expr = make_ast_ptr<ast::Binary>(op, expr, multiplicative());
        continue;
      }
      default: {
        break;
      }
    }
    break;
  }
  return expr;
}

Parser::ast_ptr<ast::Object> Parser::binary(const ast_ptr<ast::Object>& ptr) noexcept(false) {
  if (end_of_expression()) {
    return ptr;
  }
  const token_t op = peek().type;
  return make_ast_ptr<ast::Binary>(op, ptr, additive());
}

Parser::ast_ptr<ast::Object> Parser::unary() noexcept(false) {
  const token_t op = previous().type;
  return make_ast_ptr<ast::Unary>(op, primary());
}

Parser::ast_ptr<ast::Block> Parser::block() noexcept(false) {
  require({token_t::LEFT_BRACE});
  std::vector<ast_ptr<ast::Object>> statements;
  while (current().type != token_t::RIGHT_BRACE) {
    statements.push_back(primary());
    if (is_block_statement(statements.back())) {
      continue;
    }
    require({token_t::SEMICOLON});
  }
  require({token_t::RIGHT_BRACE});
  return make_ast_ptr<ast::Block>(std::move(statements));
}

Parser::ast_ptr<ast::Object> Parser::array() noexcept(false) {
  std::vector<ast_ptr<ast::Object>> objects;
  if (current().type == token_t::RIGHT_BOX_BRACE) {
    require({token_t::RIGHT_BOX_BRACE});
    return make_ast_ptr<ast::Array>(objects);
  }
  while (true) {
    objects.emplace_back(primary());
    const auto term = require({token_t::RIGHT_BOX_BRACE, token_t::COMMA});
    if (term.type == token_t::RIGHT_BOX_BRACE) {
      break;
    }
  }
  return make_ast_ptr<ast::Array>(std::move(objects));
}

Parser::ast_ptr<ast::Object> Parser::if_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  auto exit_condition = primary();
  require({token_t::RIGHT_PAREN});
  auto body = block();
  if (match({token_t::ELSE})) {
    auto else_body = block();
    return make_ast_ptr<ast::If>(std::move(exit_condition), std::move(body), std::move(else_body));
  } else {
    return make_ast_ptr<ast::If>(std::move(exit_condition), std::move(body));
  }
}

Parser::ast_ptr<ast::Object> Parser::while_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  auto exit_condition = primary();
  require({token_t::RIGHT_PAREN});
  auto body = block();
  return make_ast_ptr<ast::While>(std::move(exit_condition), std::move(body));
}

Parser::ast_ptr<ast::Object> Parser::for_statement() noexcept(false) {
  require({token_t::LEFT_PAREN});
  ast_ptr<ast::Object> init;
  ast_ptr<ast::Object> exit_condition;
  ast_ptr<ast::Object> increment;
  if (!match({token_t::SEMICOLON})) {
    init = primary();
    require({token_t::SEMICOLON});
  }
  if (!match({token_t::SEMICOLON})) {
    exit_condition = primary();
    require({token_t::SEMICOLON});
  }
  if (!match({token_t::RIGHT_PAREN})) {
    increment = primary();
    require({token_t::RIGHT_PAREN});
  }
  ast_ptr<ast::Block> for_body = block();
  ast_ptr<ast::For> for_ast = make_ast_ptr<ast::For>();
  if (init) {
    for_ast->set_init(std::move(init));
  }
  if (exit_condition) {
    for_ast->set_exit_condition(std::move(exit_condition));
  }
  if (increment) {
    for_ast->set_increment(std::move(increment));
  }
  for_ast->set_body(std::move(for_body));
  return for_ast;
};

Parser::ast_ptr<ast::Object> Parser::lambda_declare_statement() noexcept(false) {
  const Token symbol = require({token_t::SYMBOL});
  const std::string lambda_name = symbol.data;
  require({token_t::LEFT_PAREN});
  std::vector<ast_ptr<ast::Object>> arguments;
  if (!match({token_t::RIGHT_PAREN})) {
    while (true) {
      if (current().type != token_t::SYMBOL) {
        throw ParseError("Symbol as lambda parameter expected");
      } else {
        arguments.emplace_back(make_ast_ptr<ast::Symbol>(current().data));
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
  return make_ast_ptr<ast::Lambda>(lambda_name, std::move(arguments), std::move(lambda_body));
}

Parser::ast_ptr<ast::Object> Parser::define_type_statement() noexcept(false) {
  std::string name = require({token_t::SYMBOL}).data;
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
  return make_ast_ptr<ast::TypeDefinition>(std::move(name), std::move(fields));
}

std::vector<Parser::ast_ptr<ast::Object>> Parser::resolve_lambda_arguments() noexcept(false) {
  require({token_t::LEFT_PAREN});
  if (match({token_t::RIGHT_PAREN})) {
    return {};
  }
  std::vector<ast_ptr<ast::Object>> arguments;
  while (true) {
    arguments.push_back(primary());
    const auto term = require({token_t::RIGHT_PAREN, token_t::COMMA});
    if (term.type != token_t::COMMA) {
      break;
    }
  };
  return arguments;
}

Parser::ast_ptr<ast::Object> Parser::resolve_type_field_operator() noexcept(false) {
  std::string symbol = previous().data;
  require({token_t::DOT});
  peek();
  std::string field = previous().data;
  return make_ast_ptr<ast::TypeFieldOperator>(std::move(symbol), std::move(field));
}

Parser::ast_ptr<ast::Object> Parser::resolve_symbol() noexcept(false) {
  switch (current().type) {
    case token_t::LEFT_PAREN: {
      std::string data = previous().data;
      return make_ast_ptr<ast::LambdaCall>(std::move(data), resolve_lambda_arguments());
    }
    case token_t::DOT: {
      return resolve_type_field_operator();
    }
    default: {
      return binary(make_ast_ptr<ast::Symbol>(previous().data));
    }
  }
}

Parser::ast_ptr<ast::Object> Parser::resolve_braced_expression() noexcept(false) {
  --current_index_;
  require({token_t::LEFT_PAREN});
  auto expr = primary();
  require({token_t::RIGHT_PAREN});
  return expr;
}

Parser::ast_ptr<ast::Object> Parser::type_creator() noexcept(false) {
  std::string name = peek().data;
  if (current().type == token_t::LEFT_PAREN) {
    return make_ast_ptr<ast::TypeCreator>(std::move(name), resolve_lambda_arguments());
  }
  throw ParseError("'(' expected");
}
