#include "../../include/parser/parser.hpp"


Parser::Parser(std::vector<Lexeme> lexemes)
    : m_input(std::move(lexemes))
    , m_current_index(0)
{ }

std::shared_ptr<ast::RootObject> Parser::parse()
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

std::shared_ptr<ast::Object> Parser::primary()
{
    peek();

    switch (previous().type)
    {
        case lexeme_t::kw_if:
            return if_statement();

        case lexeme_t::kw_while:
            return while_statement();

        case lexeme_t::kw_for:
            return for_statement();

        case lexeme_t::kw_function_decl:
            return function_declare_statement();

         case lexeme_t::kw_define_type:
             return define_type_statement();

        case lexeme_t::left_brace:
            return block();

        case lexeme_t::left_box_brace:
            return array();

        case lexeme_t::num:
            return binary(std::make_shared<ast::Integer>(previous().data));

        case lexeme_t::floating_point:
            return binary(std::make_shared<ast::Float>(previous().data));

        case lexeme_t::string_literal:
            return std::make_shared<ast::String>(previous().data);

        case lexeme_t::symbol:
            return resolve_symbol();

        case lexeme_t::minus:
        case lexeme_t::negation:
        case lexeme_t::inc:
        case lexeme_t::dec:
            return unary();

        default:
            throw ParseError("Unknown expression: " + dispatch_lexeme(previous().type));
    }
}

const Lexeme& Parser::current() const
{
    return m_input.at(m_current_index);
}

const Lexeme& Parser::previous() const
{
    return m_input.at(m_current_index - 1);
}

const Lexeme& Parser::peek()
{
    return m_input.at(m_current_index++);
}

bool Parser::end_of_expression() const noexcept
{
    return current().type == lexeme_t::semicolon
        || current().type == lexeme_t::right_paren
        || current().type == lexeme_t::comma
        || current().type == lexeme_t::right_box_brace
        || !has_next();
}

bool Parser::has_next() const noexcept
{
    return m_current_index < m_input.size() && current().type != lexeme_t::end_of_data;
}

bool Parser::is_block(const std::shared_ptr<ast::Object>& statement) noexcept
{
    return std::dynamic_pointer_cast<ast::Block>(statement).operator bool();
}

bool Parser::is_block_statement(const std::shared_ptr<ast::Object>& statement) noexcept
{
    return std::dynamic_pointer_cast<ast::If>(statement).operator bool()
        || std::dynamic_pointer_cast<ast::While>(statement).operator bool()
        || std::dynamic_pointer_cast<ast::Function>(statement).operator bool()
        || std::dynamic_pointer_cast<ast::For>(statement).operator bool();
}

std::optional<Lexeme> Parser::match(const std::vector<lexeme_t>& expected_types)
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

Lexeme Parser::require(const std::vector<lexeme_t>& expected_types)
{
    if (const std::optional<Lexeme> lexeme = match(expected_types))
    {
        return lexeme.value();
    }
    else {
        throw ParseError(dispatch_lexeme(expected_types[0]) + " expected, got " + dispatch_lexeme(current().type));
    }
}

std::shared_ptr<ast::Object> Parser::additive()
{
    auto expr = multiplicative();

    while (true)
    {
        if (previous().type == lexeme_t::plus)
        {
            expr = std::make_shared<ast::Binary>(lexeme_t::plus, expr, multiplicative());
            continue;
        }

        if (previous().type == lexeme_t::minus)
        {
            expr = std::make_shared<ast::Binary>(lexeme_t::minus, expr, multiplicative());
            continue;
        }

        break;
    }

    return expr;
}

std::shared_ptr<ast::Object> Parser::multiplicative()
{
    auto expr = primary();

    while (true)
    {
        if (previous().type == lexeme_t::star)
        {
            expr = std::make_shared<ast::Binary>(lexeme_t::star, expr, multiplicative());
            continue;
        }

        if (previous().type == lexeme_t::slash)
        {
            expr = std::make_shared<ast::Binary>(lexeme_t::slash, expr, multiplicative());
            continue;
        }

        if (previous().type == lexeme_t::mod)
        {
            expr = std::make_shared<ast::Binary>(lexeme_t::mod, expr, multiplicative());
            continue;
        }

        break;
    }

    return expr;
}

std::shared_ptr<ast::Object> Parser::binary(const std::shared_ptr<ast::Object>& ptr)
{
    if (end_of_expression()) { return ptr; }

    peek();

    return std::make_shared<ast::Binary>(previous().type, ptr, additive());
}

std::shared_ptr<ast::Object> Parser::unary()
{
    return std::make_shared<ast::Unary>(previous().type, primary());
}

std::shared_ptr<ast::Block> Parser::block()
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

std::shared_ptr<ast::Object> Parser::array()
{
    std::vector<std::shared_ptr<ast::Object>> objects;

    while (true)
    {
        objects.emplace_back(primary());

        auto term = require({lexeme_t::right_box_brace, lexeme_t::comma});

        if (term.type == lexeme_t::comma)
        {
            continue;
        }
        else if (term.type == lexeme_t::right_box_brace) {

            break;
        }
    }

    return std::make_shared<ast::Array>(std::move(objects));
}

std::shared_ptr<ast::Object> Parser::if_statement()
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

std::shared_ptr<ast::Object> Parser::while_statement()
{
    require({lexeme_t::left_paren});

    auto while_condition = primary();

    require({lexeme_t::right_paren});

    require({lexeme_t::left_brace});

    auto while_body = block();

    require({lexeme_t::right_brace});

    return std::make_shared<ast::While>(std::move(while_condition), std::move(while_body));
}

std::shared_ptr<ast::Object> Parser::for_statement()
{
    require({lexeme_t::left_paren});

    std::shared_ptr<ast::Object> for_init;
    std::shared_ptr<ast::Object> for_exit_condition;
    std::shared_ptr<ast::Object> for_increment;

    if (!match({lexeme_t::semicolon}))
    {
        for_init = primary();
        require({lexeme_t::semicolon});
    }

    if (!match({lexeme_t::semicolon}))
    {
        for_exit_condition = primary();
        require({lexeme_t::semicolon});
    }

    if (!match({lexeme_t::right_paren}))
    {
        for_increment = primary();
        require({lexeme_t::right_paren});
    }

    require({lexeme_t::left_brace});

    auto for_body = block();

    require({lexeme_t::right_brace});

    auto for_statement = std::make_shared<ast::For>();

    if (for_init)
        for_statement->set_init(std::move(for_init));

    if (for_exit_condition)
        for_statement->set_exit_condition(std::move(for_exit_condition));

    if (for_increment)
        for_statement->set_increment(std::move(for_increment));

    for_statement->set_body(std::move(for_body));

    return for_statement;
};

std::shared_ptr<ast::Object> Parser::function_declare_statement()
{
    const Lexeme symbol = require({lexeme_t::symbol});

    std::string function_name = symbol.data;

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

std::shared_ptr<ast::Object> Parser::define_type_statement()
{
    /// define-type structure(a, b, c);
    /// define-type app(
    ///     model,
    ///     view,
    ///     controller
    /// );
    ///
    /// fun main() {
    ///     object = make-app(1, 2, 3);
    ///
    ///     object.model = make-model(...);
    /// }
    std::string name = require({lexeme_t::symbol}).data;

    require({lexeme_t::left_paren});

    std::vector<std::string> fields;

    while (true)
    {
        if (current().type != lexeme_t::symbol)
        {
            throw ParseError("Symbol as type field expected");
        }
        else {
            fields.push_back(current().data);
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

    return std::make_shared<ast::TypeDefinition>(std::move(name), std::move(fields));
}

std::vector<std::shared_ptr<ast::Object>> Parser::resolve_function_arguments()
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

std::shared_ptr<ast::Object> Parser::resolve_array_subscript()
{
    std::string symbol_name = previous().data;

    require({lexeme_t::left_box_brace});

    auto term = primary();

    require({lexeme_t::right_box_brace});

    return std::make_shared<ast::ArraySubscriptOperator>(symbol_name, std::move(term));
}

std::shared_ptr<ast::Object> Parser::resolve_symbol()
{
    if (current().type == lexeme_t::left_paren)
    {
        return std::make_shared<ast::FunctionCall>(previous().data, resolve_function_arguments());
    }
    else if (current().type == lexeme_t::left_box_brace) {

        return resolve_array_subscript();
    }
    else {
        return binary(std::make_shared<ast::Symbol>(previous().data));
    }
}
