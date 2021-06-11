#ifndef LEXER_BUILDER_HPP
#define LEXER_BUILDER_HPP

#include "lexer.hpp"

class LexerBuilder
{
public:
    LexerBuilder& operators(std::unordered_map<std::string, lexeme_t> operators)
    {
        m_operators = std::move(operators);
        return *this;
    }

    LexerBuilder& keywords(std::unordered_map<std::string, lexeme_t> keywords)
    {
        m_keywords = std::move(keywords);
        return *this;
    }

    LexerBuilder& input(std::istringstream input)
    {
        m_input = std::move(input);
        return *this;
    }

    Lexer build()
    {
        return Lexer{
            std::move(m_keywords),
            std::move(m_operators),
            std::move(m_input)
        };
    }

private:
    std::unordered_map<std::string, lexeme_t> m_operators;
    std::unordered_map<std::string, lexeme_t> m_keywords;
    std::istringstream m_input;
};

#endif // LEXER_BUILDER_HPP
