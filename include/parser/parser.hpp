#ifndef PARSER_HPP
#define PARSER_HPP

#include <optional>

#include "../lexer/lexeme.hpp"
#include "../parser/expression.hpp"
#include "../parser/parse_error.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Lexeme> lexemes)
        : m_input(std::move(lexemes))
    { }

    std::optional<Lexeme> match(const std::vector<lexeme_t>& expected_types)
    {
        if (has_next())
        {
            Lexeme token = current();

            if (std::find(expected_types.begin(), expected_types.end(), token.type) != expected_types.end())
            {
                peek();
                return token;
            }
        }

        return std::nullopt;
    }

    Lexeme require(const std::vector<lexeme_t>& expected_types)
    {
        if (const std::optional<Lexeme> lexeme = match(expected_types))
        {
            return lexeme.value();
        }
        else {
            throw ParseError(dispatch_lexeme(expected_types[0]) + " expected");
        }
    }

    std::shared_ptr<expression::Object> additive(std::shared_ptr<expression::Object> ptr)
    {
        auto expr = multiplicative(ptr);

        while (true)
        {
            if (previous().type == lexeme_t::plus)
            {
                expr = std::make_shared<expression::Binary>(lexeme_t::plus, ptr, multiplicative(ptr));
                continue;
            }

            if (previous().type == lexeme_t::minus)
            {
                expr = std::make_shared<expression::Binary>(lexeme_t::minus, ptr, multiplicative(ptr));
                continue;
            }

            break;
        }

        return expr;
    }

    std::shared_ptr<expression::Object> multiplicative(std::shared_ptr<expression::Object> ptr)
    {
        auto expr = primary();

        while (true)
        {
            if (previous().type == lexeme_t::star)
            {
                expr = std::make_shared<expression::Binary>(lexeme_t::star, ptr, multiplicative(ptr));
                continue;
            }

            if (previous().type == lexeme_t::slash)
            {
                expr = std::make_shared<expression::Binary>(lexeme_t::slash, ptr, multiplicative(ptr));
                continue;
            }

            break;
        }

        return expr;
    }

    std::shared_ptr<expression::Object> binary(std::shared_ptr<expression::Object> ptr)
    {
        if (current().type == lexeme_t::semicolon || !has_next())
        {
            return ptr;
        }

        lexeme_t op = current().type;

        peek();

        return std::make_shared<expression::Binary>(op, ptr, additive(ptr));
    }

    std::shared_ptr<expression::Object> primary()
    {
        peek();

        switch (previous().type)
        {
            case lexeme_t::left_brace:
                break;

            case lexeme_t::num:
                return binary(std::make_shared<expression::Number>(previous().data));

            case lexeme_t::string_literal:
                return std::make_shared<expression::String>(previous().data);

            case lexeme_t::symbol:
                return binary(std::make_shared<expression::Symbol>(previous().data));

            default:
                throw ParseError("Unknown expression: " + dispatch_lexeme(previous().type));
        }
    }

    std::shared_ptr<expression::Object> block()
    {
        std::vector<std::shared_ptr<expression::Object>> stmts;

        while (current().type != lexeme_t::right_brace)
        {
            stmts.push_back(primary());
            require({lexeme_t::semicolon});
        }
        peek();

        return std::make_shared<expression::Block>(std::move(stmts));
    }

    std::shared_ptr<expression::RootObject> parse()
    {
        std::shared_ptr<expression::RootObject> root = std::make_shared<expression::RootObject>();

        while (has_next())
        {
            std::shared_ptr<expression::Object> expression = primary();

            if (previous().type == lexeme_t::left_brace)
            {
                root->add(block());
            }
            else {
                require({lexeme_t::semicolon});
            }

            root->add(expression);
        }

        return root;
    }

private:
    const Lexeme& current() const
    {
        return m_input.at(m_current_index);
    }

    const Lexeme& previous() const
    {
        return m_input.at(m_current_index - 1);
    }

    const Lexeme& peek()
    {
        return m_input.at(m_current_index++);
    }

    bool has_next() const noexcept
    {
        return m_current_index < m_input.size() && m_input[m_current_index].type != lexeme_t::end_of_data;
    }

    std::size_t m_current_index{0};
    std::vector<Lexeme> m_input;
};

#endif // PARSER_HPP
