#ifndef LEXER_HPP
#define LEXER_HPP

#include <sstream>
#include <vector>
#include <unordered_map>

#include "lexeme.hpp"
#include "lexical_error.hpp"


/// Lexical analyzer. Keywords and accepted operators configured by user.
///
/// @note Operator matching has the following preconditions:
///   Longest operator must have a set of its shortened versions until the
///   length of operator is 1.
///   For example, to parse operator "<<==", operator set must have also
///   operators "<<=", "<<" and "<".
class Lexer
{
public:
    /// @pre    m_input is constructed
    /// @post   m_current_index == m_input size. It means that all symbols were processed
    /// @throw  LexicalError if the analyzed data is incorrect
    /// @return correct Lexeme's array
    std::vector<Lexeme> tokenize();

private:
    friend class LexerBuilder;

    /// Used in LexerBuilder
    Lexer(std::unordered_map<std::string, lexeme_t> keywords, std::unordered_map<std::string, lexeme_t> operators, std::istringstream data);

    char current() const;

    char previous() const;

    char peek();

    bool has_next() const noexcept;

    static bool is_alphanumeric(char token) noexcept;

    /// @pre    previous() returns digit
    /// @post   m_current_index points to first element after number literal (with dot or not)
    /// @throw  LexicalError if digit does not match the pattern \b\d+(\.\d+)?\b
    /// @return correct digit
    Lexeme process_digit();

    /// @pre    previous() returns first character after opening quote
    /// @post   m_current_index points to closing quote
    /// @throw  LexicalError if no closing quote was found
    /// @return string literal content without quotes
    Lexeme process_string_literal();

    /// @pre    previous() returns alphanumeric ([a-zA-Z0-9_])
    /// @post   m_current_index points to whitespace or operator after symbol
    /// @return keyword lexeme if it presented in an keyword map, symbol otherwise
    Lexeme process_symbol() noexcept;

    /// @pre    previous() returns operator
    /// @post   m_current_index points to first element after longest parsed operator
    /// @throw  LexicalError error if operator not found
    /// @return the longest parsed operator
    Lexeme process_operator();

    const std::unordered_map<std::string, lexeme_t> m_keywords;
    const std::unordered_map<std::string, lexeme_t> m_operators;

    std::size_t m_current_index{0};
    std::vector<char> m_input;
};

#endif // LEXER_HPP