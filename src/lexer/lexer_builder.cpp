#include "../../include/lexer/lexer_builder.hpp"

LexerBuilder& LexerBuilder::operators(std::unordered_map<std::string, lexeme_t> operators)
{
    m_operators = std::move(operators);
    return *this;
}

LexerBuilder& LexerBuilder::keywords(std::unordered_map<std::string, lexeme_t> keywords)
{
    m_keywords = std::move(keywords);
    return *this;
}

LexerBuilder& LexerBuilder::input(std::istringstream input)
{
    m_input = std::move(input);
    return *this;
}

Lexer LexerBuilder::build()
{
    return Lexer{
        std::move(m_keywords),
        std::move(m_operators),
        std::move(m_input)
    };
}