#ifndef WEAK_PARSER_HPP
#define WEAK_PARSER_HPP

#include "../ast/ast.hpp"
#include "../error/parse_error.hpp"
#include "../lexer/token.hpp"

#include <boost/pool/pool_alloc.hpp>
#include <optional>

/// LL Syntax analyzer.
class Parser {
public:
  explicit Parser(std::vector<Token> tokens) noexcept(true);

  boost::local_shared_ptr<ast::RootObject> parse() noexcept(false);

private:
  /// @throws std::out_of_range
  const Token& current() const noexcept(false);

  /// @throws std::out_of_range
  const Token& previous() const noexcept(false);

  /// @throws std::out_of_range
  const Token& peek() noexcept(false);

  /// @return true if current token is ';', ')' or ','
  bool end_of_expression() const noexcept(true);

  /// @note   don't throw because guarantees that valid index will be passed to at() method of m_input array
  /// @throws std::out_of_range from current()
  /// @return true if peek() returns an meaningful token
  bool has_next() const noexcept(false);

  /// @return true if statement is raw block
  static bool is_block(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true);

  /// @return true if statement is block-based statement, but not block
  static bool is_block_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true);

  /// @throws std::out_of_range from current() and peek()
  /// @brief  get current token and match with one of samples
  /// @return correct token, std::nullopt if input find no more tokens
  std::optional<Token> match(const std::vector<token_t>& expected_types) noexcept(false);

  /// @throws std::out_of_range from match()
  /// @throws ParseError if the match was unsuccessful
  /// @return correct token of one of expected types
  Token require(const std::vector<token_t>& expected_types) noexcept(false);

  /// @brief main parse lambda
  boost::local_shared_ptr<ast::Object> primary() noexcept(false);

  /// @return array parse tree
  boost::local_shared_ptr<ast::Object> array() noexcept(false);

  /// @return parse tree if ptr is additive operation, unchanged ptr otherwise
  boost::local_shared_ptr<ast::Object> additive() noexcept(false);

  /// @return parse tree if ptr is multiplicative operation, unchanged ptr otherwise
  boost::local_shared_ptr<ast::Object> multiplicative() noexcept(false);

  /// @note   this lambda does not check operation types
  /// @pre    previous() returns number or symbol token
  /// @post   previous() returns first token after parsed binary expression
  /// @return binary parse tree
  boost::local_shared_ptr<ast::Object> binary(const boost::local_shared_ptr<ast::Object>& ptr) noexcept(false);

  /// @pre    previous() returns unary operator
  /// @post   previous() returns first token after parsed unary expression
  boost::local_shared_ptr<ast::Object> unary() noexcept(false);

  /// @pre    previous() returns '{' token
  /// @post   current() returns '}' token
  /// @return block parse tree of recursively parsed expressions
  boost::local_shared_ptr<ast::Block> block() noexcept(false);

  /// @pre    previous() returns 'if' token
  /// @post   previous() returns first token after parsed if
  /// @return if parse tree with or without else block
  boost::local_shared_ptr<ast::Object> if_statement() noexcept(false);

  /// @pre    previous() returns 'while' token
  /// @post   previous() returns first token after parsed while
  /// @return while parse tree
  boost::local_shared_ptr<ast::Object> while_statement() noexcept(false);

  /// @note   any number of C-style for blocks (for ( 1 ; 2 ; 3 )) can be empty
  /// @pre    previous() returns 'for' token
  /// @post   previous() returns first token after parsed for
  /// @return for parse tree
  boost::local_shared_ptr<ast::Object> for_statement() noexcept(false);

  /// @pre    previous() returns 'fun' token
  /// @post   previous() returns first token after lambda declaration
  /// @return lambda parse tree that contains lambda type_name, argument list and body (block)
  boost::local_shared_ptr<ast::Object> lambda_declare_statement() noexcept(false);

  /// @pre    previous() returns 'define-type' token
  /// @post   previous() returns first token after type definition
  /// @return parsed type definition with only field names
  boost::local_shared_ptr<ast::Object> define_type_statement() noexcept(false);

  /// @pre    previous() returns '(' token
  /// @post   previous() returns ')' token
  /// @return correct lambda argument list
  std::vector<boost::local_shared_ptr<ast::Object>> resolve_lambda_arguments() noexcept(false);

  /// @pre    previous() returns symbol token
  /// @post   previous() returns ')' token if lambda call argument processed, symbol token otherwise
  /// @return symbol object, lambda call object if '(' token placed after symbol
  boost::local_shared_ptr<ast::Object> resolve_symbol() noexcept(false);

  boost::local_shared_ptr<ast::Object> resolve_braced_expression() noexcept(false);

  /// @pre    previous() returns symbol token
  /// @post   previous() returns token after type field
  /// @return field access parse tree
  boost::local_shared_ptr<ast::Object> resolve_type_field_operator() noexcept(false);

  boost::local_shared_ptr<ast::Object> type_creator() noexcept(false);

  std::vector<Token> input_;
  size_t current_index_;
};

#endif// WEAK_PARSER_HPP
