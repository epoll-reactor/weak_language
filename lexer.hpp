#ifndef LEXER_HPP
#define LEXER_HPP

#include <sstream>
#include <vector>
#include <unordered_map>

#include "lexeme.hpp"


#define LOG(x) (std::cout << x << '\n');

class Lexer
{
public:
    explicit Lexer(std::istringstream data)
        : m_input((std::istreambuf_iterator<char>(data)), std::istreambuf_iterator<char>())
    { }

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
                    while (has_next() && isspace(current()))
                    {
                        peek();
                    }
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
    char current()
    {
        return m_input.at(m_current_index);
    }

    char previous()
    {
        return m_input.at(m_current_index - 1);
    }

    char peek()
    {
        return m_input.at(m_current_index++);
    }

    bool has_next()
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

    static bool is_alphanumeric(char token)
    {
        return isalpha(token) || token == '_';
    }

    Lexeme process_symbol()
    {
        std::string symbol(1, previous());

        while (has_next() && (is_alphanumeric(current()) || isdigit(current())))
        {
            symbol += peek();
        }

        return Lexeme{std::move(symbol), lexeme_t::symbol};
    }

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
        {",", lexeme_t::comma}
    };

    std::size_t m_current_index{0};
    std::vector<char> m_input;
};

#endif // LEXER_HPP