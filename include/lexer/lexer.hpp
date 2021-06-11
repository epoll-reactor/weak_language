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
    std::vector<Lexeme> tokenize()
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

        std::vector<Lexeme> lexemes;

        while (has_next())
        {
            peek();

            switch (previous())
            {
                case  ' ':
                case '\n':
                case '\r':
                case '\f':
                case '\v':
                case '\t':
                    // Skip \s
                    break;

                case '0'...'9':
                    lexemes.emplace_back(process_digit());
                    break;

                case '_':
                case 'a'...'z':
                case 'A'...'Z':
                    lexemes.emplace_back(process_symbol());
                    break;

                case '\"':
                    peek(); // Eat opening '"'
                    lexemes.emplace_back(process_string_literal());
                    peek(); // Eat closing '"'
                    break;

                default:
                    if (std::find_if(m_operators.begin(), m_operators.end(), [this](const std::pair<std::string, lexeme_t>& pair) {
                        return pair.first[0] == previous();
                    }) != m_operators.end())
                    {
                        lexemes.emplace_back(process_operator());
                    }
                    else {
                        using namespace std::string_literals;
                        throw LexicalError("Unknown symbol: ("s + previous() + ", " + std::to_string(static_cast<int>(previous())) + ")");
                    }
            }
        }

        lexemes.emplace_back(Lexeme{"", lexeme_t::end_of_data});

        return lexemes;
#pragma GCC diagnostic pop
    }

private:
    friend class LexerBuilder;

    /// Used in LexerBuilder
    Lexer(std::unordered_map<std::string, lexeme_t> keywords, std::unordered_map<std::string, lexeme_t> operators, std::istringstream data)
        : m_keywords(std::move(keywords))
        , m_operators(std::move(operators))
        , m_input(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>())
    { }

    char current() const
    {
        return m_input.at(m_current_index);
    }

    char previous() const
    {
        return m_input.at(m_current_index - 1);
    }

    char peek()
    {
        return m_input.at(m_current_index++);
    }

    bool has_next() const noexcept
    {
        return m_current_index < m_input.size();
    }

    static bool is_alphanumeric(char token) noexcept
    {
        return isalpha(token) || token == '_';
    }

    static void lexical_error_if(bool condition, std::string_view message)
    {
        if (condition)
        {
            throw LexicalError(message);
        }
    }

    /// @pre    previous() returns digit
    /// @post   m_current_index points to first element after number literal (with dot or not)
    /// @throw  LexicalError if digit does not match the pattern \b\d+(\.\d+)?\b
    /// @return correct digit
    Lexeme process_digit()
    {
        std::string digit(1, previous());
        std::size_t dots_reached = 0;

        while (has_next() && (isdigit(current()) || current() == '.'))
        {
            if (current() == '.')
                dots_reached++;

            digit += peek();
        }

        lexical_error_if(has_next() && is_alphanumeric(current()), "Symbol can't start with digit");
        lexical_error_if(dots_reached > 1, "Extra \".\" detected");
        lexical_error_if(previous() == '.', "Digit after \".\" expected");

        return Lexeme{std::move(digit), lexeme_t::num};
    }

    /// @pre    previous() returns first character after opening quote
    /// @post   m_current_index points to closing quote
    /// @throw  LexicalError if no closing quote was found
    /// @return string literal content without quotes
    Lexeme process_string_literal()
    {
        std::string literal(1, previous());

        while (has_next() && current() != '\"')
        {
            if (current() == '\\')
                peek();

            lexical_error_if(current() == '\0', "Closing '\\\"' expected");

            literal += peek();
        }

        return Lexeme{std::move(literal), lexeme_t::string_literal};
    }

    /// @pre    previous() returns alphanumeric ([a-zA-Z0-9_])
    /// @post   m_current_index points to whitespace or operator after symbol
    /// @return keyword lexeme if it presented in an keyword map, symbol otherwise
    Lexeme process_symbol() noexcept
    {
        std::string symbol(1, previous());

        while (has_next() && (is_alphanumeric(current()) || isdigit(current())))
        {
            symbol += peek();
        }

        if (m_keywords.find(symbol) != m_keywords.end())
        {
            return Lexeme{"", m_keywords.at(symbol)};
        }
        else {
            return Lexeme{std::move(symbol), lexeme_t::symbol};
        }
    }

    /// @pre    previous() returns operator
    /// @post   m_current_index points to first element after longest parsed operator
    /// @throw  LexicalError error if operator not found
    /// @return the longest parsed operator
    Lexeme process_operator()
    {
        std::string op(1, previous());
        lexeme_t type;

        while (has_next() && m_operators.find(op) != m_operators.end())
        {
            op += peek();

            if (m_operators.find(op) == m_operators.end())
            {
                op.pop_back();
                --m_current_index;
                break;
            }
        }

        type = m_operators.at(op);

        return Lexeme{"", type};
    }

    const std::unordered_map<std::string, lexeme_t> m_keywords;
    const std::unordered_map<std::string, lexeme_t> m_operators;

    std::size_t m_current_index{0};
    std::vector<char> m_input;
};

#endif // LEXER_HPP