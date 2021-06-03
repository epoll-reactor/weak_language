#ifndef LEXER_HPP
#define LEXER_HPP

#include <sstream>
#include <vector>
#include <unordered_map>

#include "lexeme.hpp"


class Lexer
{
public:
    explicit Lexer(std::istringstream data)
        : m_input((std::istreambuf_iterator<char>(data)), std::istreambuf_iterator<char>())
    { }

    /// @pre    m_input is constructed
    /// @post   m_current_index == m_input size. It means that all symbols were processed
    /// @throw  std::runtime_error if the analyzed data is incorrect
    /// @return correct Lexeme's array
    std::vector<Lexeme> tokenize()
    {
        std::vector<Lexeme> lexemes;

        while (has_next())
        {
            peek();

            switch (previous())
            {
                case ' ':
                case '\n':
                case '\r':
                case '\f':
                case '\v':
                case '\t':
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
                    lexemes.emplace_back(process_operator());
                    break;
            }
        }

        lexemes.emplace_back(Lexeme{"", lexeme_t::end_of_data});

        return lexemes;
    }

private:
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

    static void syntax_error_if(bool condition, std::string_view message)
    {
        if (condition)
        {
            throw std::runtime_error(message.data());
        }
    }

    /// @pre    previous() returns digit
    /// @post   m_current_index points to first element after number literal (with dot or not)
    /// @throw  std::runtime if digit does not match the pattern \b\d+(\.\d+)?\b
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

        syntax_error_if(has_next() && is_alphanumeric(current()), "Symbol can't start with digit");
        syntax_error_if(dots_reached > 1, "Extra \".\" detected");
        syntax_error_if(previous() == '.', "Digit after \".\" expected");

        return Lexeme{std::move(digit), lexeme_t::num};
    }

    /// @pre    previous() returns first character after opening quote
    /// @post   m_current_index points to closing quote
    /// @throw  std::runtime_error if no closing quote was found
    /// @return string literal content without quotes
    Lexeme process_string_literal()
    {
        std::string literal(1, previous());

        while (has_next() && current() != '\"')
        {
            if (current() == '\\')
                peek();

            syntax_error_if(current() == '\0', "Closing '\\\"' expected");

            literal += peek();
        }

        return Lexeme{std::move(literal), lexeme_t::string_literal};
    }

    static bool is_alphanumeric(char token) noexcept
    {
        return isalpha(token) || token == '_';
    }

    /// @pre    previous() returns alphanumeric ([a-zA-Z0-9_])
    /// @post   m_current_index points to whitespace or operator after symbol
    Lexeme process_symbol() noexcept
    {
        std::string symbol(1, previous());

        while (has_next() && (is_alphanumeric(current()) || isdigit(current())))
        {
            symbol += peek();
        }

        return Lexeme{std::move(symbol), lexeme_t::symbol};
    }

    /// @pre    previous() returns operator
    /// @post   m_current_index points to first element after longest parsed operator
    /// @throw  std::runtime error if operator not found
    /// @return the longest parsed operator
    Lexeme process_operator()
    {
        std::string op(1, previous());
        lexeme_t type;

        syntax_error_if(m_operators.find(op) == m_operators.end(), "Unknown operator: " + op);

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

    const std::unordered_map<std::string, lexeme_t> m_operators {
        {"++", lexeme_t::inc},
        {"--", lexeme_t::dec},
        {"+", lexeme_t::plus},
        {"-", lexeme_t::minus},
        {"*", lexeme_t::star},
        {"/", lexeme_t::slash},
        {"=", lexeme_t::assign},
        {"+=", lexeme_t::plus_assign},
        {"*=", lexeme_t::star_assign},
        {"-=", lexeme_t::minus_assign},
        {"/=", lexeme_t::slash_assign},

        {".", lexeme_t::dot},
        {",", lexeme_t::comma},

        {"(", lexeme_t::left_paren},
        {")", lexeme_t::right_paren},
        {"{", lexeme_t::left_brace},
        {"}", lexeme_t::right_brace},

        {":", lexeme_t::colon},
        {";", lexeme_t::semicolon}
    };

    std::size_t m_current_index{0};
    std::vector<char> m_input;
};

#endif // LEXER_HPP