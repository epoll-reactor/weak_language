#ifndef PARSER_HPP
#define PARSER_HPP

#include <optional>

#include "../lexer/lexeme.hpp"
#include "../parser/ast.hpp"
#include "../parser/parse_error.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Lexeme> lexemes)
        : m_input(std::move(lexemes))
    { }

    std::shared_ptr<ast::RootObject> parse()
    {
        std::shared_ptr<ast::RootObject> root = std::make_shared<ast::RootObject>();

        while (has_next())
        {
            if (current().type == lexeme_t::left_brace)
            {
                require({lexeme_t::left_brace});

                root->add(block());

                require({lexeme_t::right_brace});
            }
            else {
                auto expression = primary();

                if (!is_block_statement(expression))
                    require({lexeme_t::semicolon});

                root->add(std::move(expression));
            }
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

    bool end_of_expression()
    {
        return current().type == lexeme_t::semicolon
            || current().type == lexeme_t::right_paren
            || current().type == lexeme_t::comma
            || !has_next();
    }

    bool has_next() const noexcept
    {
        return m_current_index < m_input.size() && current().type != lexeme_t::end_of_data;
    }

    static bool is_block(std::shared_ptr<ast::Object> statement)
    {
        return std::dynamic_pointer_cast<ast::Block>(statement).operator bool();
    }

    static bool is_block_statement(std::shared_ptr<ast::Object> statement)
    {
        return std::dynamic_pointer_cast<ast::If>(statement).operator bool()
            || std::dynamic_pointer_cast<ast::While>(statement).operator bool()
            || std::dynamic_pointer_cast<ast::Function>(statement).operator bool();
    }

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
            throw ParseError(dispatch_lexeme(expected_types[0]) + " expected, got " + dispatch_lexeme(current().type));
        }
    }

    std::shared_ptr<ast::Object> additive(std::shared_ptr<ast::Object> ptr)
    {
        auto expr = multiplicative(ptr);

        while (true)
        {
            if (previous().type == lexeme_t::plus)
            {
                expr = std::make_shared<ast::Binary>(lexeme_t::plus, ptr, multiplicative(ptr));
                continue;
            }

            if (previous().type == lexeme_t::minus)
            {
                expr = std::make_shared<ast::Binary>(lexeme_t::minus, ptr, multiplicative(ptr));
                continue;
            }

            break;
        }

        return expr;
    }

    std::shared_ptr<ast::Object> multiplicative(std::shared_ptr<ast::Object> ptr)
    {
        auto expr = primary();

        while (true)
        {
            if (previous().type == lexeme_t::star)
            {
                expr = std::make_shared<ast::Binary>(lexeme_t::star, ptr, multiplicative(ptr));
                continue;
            }

            if (previous().type == lexeme_t::slash)
            {
                expr = std::make_shared<ast::Binary>(lexeme_t::slash, ptr, multiplicative(ptr));
                continue;
            }

            break;
        }

        return expr;
    }

    std::shared_ptr<ast::Object> binary(std::shared_ptr<ast::Object> ptr)
    {
        if (end_of_expression())
        {
            return ptr;
        }

        lexeme_t op = current().type;

        peek();

        return std::make_shared<ast::Binary>(op, ptr, additive(ptr));
    }

    std::shared_ptr<ast::Block> block()
    {
        std::vector<std::shared_ptr<ast::Object>> stmts;

        while (current().type != lexeme_t::right_brace)
        {
            auto stmt = primary();

            stmts.push_back(stmt);

            if (is_block(stmt))
            {
                require({lexeme_t::right_brace});
            }
            else if (!is_block_statement(stmt)) {

                require({lexeme_t::semicolon});
            }
        }

        return std::make_shared<ast::Block>(std::move(stmts));
    }

    std::shared_ptr<ast::Object> if_statement()
    {
        require({lexeme_t::left_paren});

        auto if_condition = primary();

        require({lexeme_t::right_paren});

        require({lexeme_t::left_brace});

        auto if_body = block();

        require({lexeme_t::right_brace});

        if (match({lexeme_t::kw_else}))
        {
            require({lexeme_t::left_brace});

            auto else_body = block();

            require({lexeme_t::right_brace});

            return std::make_shared<ast::If>(std::move(if_condition), std::move(if_body), std::move(else_body));
        }
        else {

            return std::make_shared<ast::If>(std::move(if_condition), std::move(if_body));
        }
    }

    std::shared_ptr<ast::Object> while_statement()
    {
        require({lexeme_t::left_paren});

        auto while_condition = primary();

        require({lexeme_t::right_paren});

        require({lexeme_t::left_brace});

        auto while_body = block();

        require({lexeme_t::right_brace});

        return std::make_shared<ast::While>(std::move(while_condition), std::move(while_body));
    }

    std::shared_ptr<ast::Object> function_declare_statement()
    {
        const Lexeme symbol = require({lexeme_t::symbol});

        const std::string function_name = symbol.data;

        require({lexeme_t::left_paren});

        std::vector<std::shared_ptr<ast::Object>> arguments;

        if (!match({lexeme_t::right_paren}))
        {
            while (true)
            {
                if (current().type != lexeme_t::symbol)
                {
                    throw ParseError("Symbol as function parameter expected");
                }
                else {
                    arguments.push_back(std::make_shared<ast::Symbol>(current().data));
                }

                peek();

                auto term = require({lexeme_t::right_paren, lexeme_t::comma});

                if (term.type == lexeme_t::comma)
                {
                    continue;
                }
                else if (term.type == lexeme_t::right_paren) {

                    break;
                }
            }
        }

        require({lexeme_t::left_brace});

        auto function_body = block();

        require({lexeme_t::right_brace});

        return std::make_shared<ast::Function>(std::move(function_name), std::move(arguments), std::move(function_body));
    }

    std::vector<std::shared_ptr<ast::Object>> resolve_function_arguments()
    {
        require({lexeme_t::left_paren});

        if (match({lexeme_t::right_paren}))
            return {};

        std::vector<std::shared_ptr<ast::Object>> arguments;

        while (true)
        {
            arguments.push_back(primary());

            auto term = require({lexeme_t::right_paren, lexeme_t::comma});

            if (term.type == lexeme_t::comma)
            {
                continue;
            }
            else if (term.type == lexeme_t::right_paren) {

                break;
            }
        };

        return arguments;
    }

    std::shared_ptr<ast::Object> resolve_symbol()
    {
        if (current().type == lexeme_t::left_paren)
        {
            return std::make_shared<ast::FunctionCall>(previous().data, resolve_function_arguments());
        }
        else {
            return binary(std::make_shared<ast::Symbol>(previous().data));
        }
    }

    std::shared_ptr<ast::Object> primary()
    {
        peek();

        switch (previous().type)
        {
            case lexeme_t::kw_if:
                return if_statement();

            case lexeme_t::kw_while:
                return while_statement();

            case lexeme_t::kw_function_decl:
                return function_declare_statement();

            case lexeme_t::left_brace:
                return block();

            case lexeme_t::num:
                return binary(std::make_shared<ast::Number>(previous().data));

            case lexeme_t::string_literal:
                return std::make_shared<ast::String>(previous().data);

            case lexeme_t::symbol:
                return resolve_symbol();

            default:
                throw ParseError("Unknown expression: " + dispatch_lexeme(previous().type));
        }
    }

    std::size_t m_current_index{0};
    std::vector<Lexeme> m_input;
};

#endif // PARSER_HPP