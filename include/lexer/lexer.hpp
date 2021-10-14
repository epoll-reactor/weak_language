#ifndef WEAK_LEXER_HPP
#define WEAK_LEXER_HPP

#include "../error/lexical_error.hpp"
#include "../lexer/token.hpp"

#include <unordered_map>
#include <vector>

/// Lexical analyzer. Keywords and accepted operators configured by user.
///
/// @note Operator matching find the following preconditions:
///   Longest operator must have a set of its shortened versions until the
///   length of operator is 1.
///   For example, to parse operator "<<==", operator set must have also
///   operators "<<=", "<<" and "<".
class Lexer {
public:
  Lexer(std::istringstream data);

  /// @pre    m_input is constructed
  /// @post   m_current_index == m_input size. It means that all symbols were processed
  /// @throw  LexicalError if the analyzed data is incorrect
  /// @return correct Token's array
  std::vector<Token> tokenize();

private:
  char current() const;

  char previous() const;

  char peek();

  bool has_next() const noexcept;

  static bool is_alphanumeric(char token) noexcept;

  /// @pre    previous() returns digit
  /// @post   m_current_index points to first element after number literal (with dot or not)
  /// @throw  LexicalError if digit does not match the pattern \b\d+(\.\d+)?\b
  /// @return correct digit
  Token process_digit();

  /// @pre    previous() returns first character after opening quote
  /// @post   m_current_index points to closing quote
  /// @throw  LexicalError if no closing quote was found
  /// @return string literal content without quotes
  Token process_string_literal();

  /// @pre    previous() returns alphanumeric ([a-zA-Z0-9_])
  /// @post   m_current_index points to whitespace or operator after symbol
  /// @return keyword token if it presented in an keyword map, symbol otherwise
  Token process_symbol() noexcept;

  /// @pre    previous() returns operator
  /// @post   m_current_index points to first element after longest parsed operator
  /// @throw  LexicalError error if operator not found
  /// @return the longest parsed operator
  Token process_operator();

  size_t current_index_{0};
  const std::vector<char> input_;

  const std::unordered_map<std::string, token_t>& keywords_;
  const std::unordered_map<std::string, token_t>& operators_;
};

#endif// WEAK_LEXER_HPP
