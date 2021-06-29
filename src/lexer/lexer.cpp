#include "../../include/lexer/lexer_builder.hpp"
#include "../../include/lexer/lexer.hpp"

Lexer::Lexer(std::unordered_map<std::string, lexeme_t> keywords, std::unordered_map<std::string, lexeme_t> operators, std::istringstream data)
    : m_keywords(std::move(keywords))
    , m_operators(std::move(operators))
    , m_input(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>())
{ }

char Lexer::current() const
{
    return m_input.at(m_current_index);
}

char Lexer::previous() const
{
    return m_input.at(m_current_index - 1);
}

char Lexer::peek()
{
    return m_input.at(m_current_index++);
}

bool Lexer::has_next() const noexcept
{
    return m_current_index < m_input.size();
}

bool Lexer::is_alphanumeric(char token) noexcept
{
    return isalpha(token) || token == '_' || token == '?';
}

std::vector<Lexeme> Lexer::tokenize()
{
    std::vector<Lexeme> lexemes;

    while (has_next())
    {
        peek();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
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
                lexemes.emplace_back(process_string_literal());
                break;

            default:
                if (std::find_if(m_operators.begin(), m_operators.end(), [this](const std::pair<std::string, lexeme_t>& pair) {
                    return pair.first[0] == previous();
                }) != m_operators.end())
                {
                    lexemes.emplace_back(process_operator());
                }
                else {
                    throw LexicalError(std::string("Unknown symbol: (") + previous() + ", " + std::to_string(static_cast<int>(previous())) + ")");
                }
        }
    }
#pragma GCC diagnostic pop

    lexemes.emplace_back(Lexeme{"", lexeme_t::end_of_data});

    return lexemes;
}

Lexeme Lexer::process_digit()
{
    std::string digit(1, previous());
    std::size_t dots_reached = 0;

    while (has_next() && (isdigit(current()) || current() == '.'))
    {
        if (current() == '.')
            dots_reached++;

        digit += peek();
    }

    if (has_next() && is_alphanumeric(current())) { throw LexicalError("Symbol can't start with digit"); }
    if (dots_reached > 1)  { throw LexicalError("Extra \".\" detected"); }
    if (previous() == '.') { throw LexicalError("Digit after \".\" expected"); }

    return Lexeme{std::move(digit), lexeme_t::num};
}

Lexeme Lexer::process_string_literal()
{
    peek(); /// Eat opening "

    if (previous() == '\"') { return Lexeme{"", lexeme_t::string_literal}; }

    std::string literal(1, previous());

    while (has_next() && current() != '\"')
    {
        if (current() == '\\') { peek(); }

        if (current() == '\0') { throw LexicalError("Closing '\\\"' expected"); }

        literal += peek();
    }

    peek(); /// Eat closing "

    return Lexeme{std::move(literal), lexeme_t::string_literal};
}

Lexeme Lexer::process_symbol() noexcept
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

Lexeme Lexer::process_operator()
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