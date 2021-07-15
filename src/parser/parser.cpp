#include "../../include/parser/parser.hpp"

Parser::Parser(std::vector<Lexeme> lexemes) noexcept(true)
    : m_input(std::move(lexemes))
    , m_current_index(0)
{ }

boost::local_shared_ptr<ast::RootObject> Parser::parse() noexcept(false)
{
    boost::local_shared_ptr<ast::RootObject> root = pool_allocate<ast::RootObject>();

    while (has_next())
    {
        if (current().type == token_t::left_brace)
        {
            root->add(block());
        }
        else {
            auto expression = additive();

            if (!is_block_statement(expression))
                require({token_t::semicolon});

            root->add(std::move(expression));
        }
    }

    return root;
}

boost::local_shared_ptr<ast::Object> Parser::primary() noexcept(false)
{
    peek();

    switch (previous().type)
    {
        case token_t::kw_if:
            return if_statement();

        case token_t::kw_while:
            return while_statement();

        case token_t::kw_for:
            return for_statement();

        case token_t::kw_function_decl:
            return function_declare_statement();

        case token_t::kw_define_type:
            return define_type_statement();

        case token_t::left_brace:
            return block();

        case token_t::left_box_brace:
            return array();

        case token_t::num:
            return binary(boost::make_local_shared<ast::Integer>(previous().data));

        case token_t::floating_point:
            return binary(boost::make_local_shared<ast::Float>(previous().data));

        case token_t::string_literal:
            return boost::make_local_shared<ast::String>(previous().data);

        case token_t::symbol:
            return resolve_symbol();

        case token_t::minus:
        case token_t::negation:
        case token_t::inc:
        case token_t::dec:
            return unary();

        default:
            throw ParseError("Unknown expression: " + dispatch_token(previous().type));
    }
}

const Lexeme& Parser::current() const noexcept(false)
{
    return m_input.at(m_current_index);
}

const Lexeme& Parser::previous() const noexcept(false)
{
    return m_input.at(m_current_index - 1);
}

const Lexeme& Parser::peek() noexcept(false)
{
    return m_input.at(m_current_index++);
}

bool Parser::end_of_expression() const noexcept(true)
{
    return current().type == token_t::semicolon
        || current().type == token_t::right_paren
        || current().type == token_t::comma
        || current().type == token_t::right_box_brace
        || !has_next();
}

bool Parser::has_next() const noexcept(false)
{
    return m_current_index < m_input.size() && current().type != token_t::end_of_data;
}

bool Parser::is_block(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true)
{
    return statement->ast_type() == ast::ast_type_t::BLOCK;
}

bool Parser::is_block_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(true)
{
    return statement->ast_type() == ast::ast_type_t::IF
        || statement->ast_type() == ast::ast_type_t::WHILE
        || statement->ast_type() == ast::ast_type_t::FUNCTION
        || statement->ast_type() == ast::ast_type_t::FOR;
}

std::optional<Lexeme> Parser::match(const std::vector<token_t>& expected_types) noexcept(false)
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

Lexeme Parser::require(const std::vector<token_t>& expected_types) noexcept(false)
{
    if (const std::optional<Lexeme> lexeme = match(expected_types))
    {
        return lexeme.value();
    }
    else {
        throw ParseError(dispatch_token(expected_types[0]) + " expected, got " + dispatch_token(current().type));
    }
}

boost::local_shared_ptr<ast::Object> Parser::additive() noexcept(false)
{
    auto expr = multiplicative();

    while (true)
    {
        if (previous().type == token_t::plus)
        {
            expr = boost::make_local_shared<ast::Binary>(token_t::plus, expr, multiplicative());
            continue;
        }

        if (previous().type == token_t::minus)
        {
            expr = boost::make_local_shared<ast::Binary>(token_t::minus, expr, multiplicative());
            continue;
        }

        break;
    }

    return expr;
}

boost::local_shared_ptr<ast::Object> Parser::multiplicative() noexcept(false)
{
    auto expr = primary();

    while (true)
    {
        if (previous().type == token_t::star)
        {
            expr = boost::make_local_shared<ast::Binary>(token_t::star, expr, multiplicative());
            continue;
        }

        if (previous().type == token_t::slash)
        {
            expr = boost::make_local_shared<ast::Binary>(token_t::slash, expr, multiplicative());
            continue;
        }

        if (previous().type == token_t::mod)
        {
            expr = boost::make_local_shared<ast::Binary>(token_t::mod, expr, multiplicative());
            continue;
        }

        break;
    }

    return expr;
}

boost::local_shared_ptr<ast::Object> Parser::binary(const boost::local_shared_ptr<ast::Object>& ptr) noexcept(false)
{
    if (end_of_expression()) { return ptr; }

    token_t op = peek().type;

    return boost::make_local_shared<ast::Binary>(op, ptr, additive());
}

boost::local_shared_ptr<ast::Object> Parser::unary() noexcept(false)
{
    token_t op = previous().type;

    return boost::make_local_shared<ast::Unary>(op, primary());
}

boost::local_shared_ptr<ast::Block> Parser::block() noexcept(false)
{
    require({token_t::left_brace});

    std::vector<boost::local_shared_ptr<ast::Object>> stmts;

    while (current().type != token_t::right_brace)
    {
        auto stmt = additive();

        stmts.push_back(stmt);

        if (is_block(stmt))
        {
            require({token_t::right_brace});
        }
        else if (!is_block_statement(stmt)) {

            require({token_t::semicolon});
        }
    }

    require({token_t::right_brace});

    return boost::make_local_shared<ast::Block>(std::move(stmts));
}

boost::local_shared_ptr<ast::Object> Parser::array() noexcept(false)
{
    std::vector<boost::local_shared_ptr<ast::Object>> objects;

    while (true)
    {
        objects.emplace_back(primary());

        auto term = require({token_t::right_box_brace, token_t::comma});

        if (term.type == token_t::comma)
        {
            continue;
        }
        if (term.type == token_t::right_box_brace) {

            break;
        }
    }

    return boost::make_local_shared<ast::Array>(std::move(objects));
}

boost::local_shared_ptr<ast::Object> Parser::if_statement() noexcept(false)
{
    require({token_t::left_paren});

    auto if_condition = primary();

    require({token_t::right_paren});

    auto if_body = block();

    if (match({token_t::kw_else}))
    {
        auto else_body = block();

        return boost::make_local_shared<ast::If>(std::move(if_condition), std::move(if_body), std::move(else_body));
    }
    else {

        return boost::make_local_shared<ast::If>(std::move(if_condition), std::move(if_body));
    }
}

boost::local_shared_ptr<ast::Object> Parser::while_statement() noexcept(false)
{
    require({token_t::left_paren});

    auto while_condition = primary();

    require({token_t::right_paren});

    auto while_body = block();

    return boost::make_local_shared<ast::While>(std::move(while_condition), std::move(while_body));
}

boost::local_shared_ptr<ast::Object> Parser::for_statement() noexcept(false)
{
    require({token_t::left_paren});

    boost::local_shared_ptr<ast::Object> for_init;
    boost::local_shared_ptr<ast::Object> for_exit_condition;
    boost::local_shared_ptr<ast::Object> for_increment;

    if (!match({token_t::semicolon}))
    {
        for_init = primary();
        require({token_t::semicolon});
    }

    if (!match({token_t::semicolon}))
    {
        for_exit_condition = primary();
        require({token_t::semicolon});
    }

    if (!match({token_t::right_paren}))
    {
        for_increment = primary();
        require({token_t::right_paren});
    }

    auto for_body = block();

    auto for_statement = boost::make_local_shared<ast::For>();

    if (for_init)           { for_statement->set_init(std::move(for_init)); }
    if (for_exit_condition) { for_statement->set_exit_condition(std::move(for_exit_condition)); }
    if (for_increment)      { for_statement->set_increment(std::move(for_increment)); }

    for_statement->set_body(std::move(for_body));

    return for_statement;
};

boost::local_shared_ptr<ast::Object> Parser::function_declare_statement() noexcept(false)
{
    const Lexeme symbol = require({token_t::symbol});

    std::string function_name = symbol.data;

    require({token_t::left_paren});

    std::vector<boost::local_shared_ptr<ast::Object>> arguments;

    if (!match({token_t::right_paren}))
    {
        while (true)
        {
            if (current().type != token_t::symbol)
            {
                throw ParseError("Symbol as function parameter expected");
            }
            else {
                arguments.emplace_back(boost::make_local_shared<ast::Symbol>(current().data));
            }

            peek();

            auto term = require({token_t::right_paren, token_t::comma});

            if (term.type == token_t::comma)
            {
                continue;
            }
            else if (term.type == token_t::right_paren) {

                break;
            }
        }
    }

    auto function_body = block();

    return boost::make_local_shared<ast::Function>(function_name, std::move(arguments), std::move(function_body));
}

boost::local_shared_ptr<ast::Object> Parser::define_type_statement() noexcept(false)
{
    std::string name = require({token_t::symbol}).data;

    require({token_t::left_paren});

    std::vector<std::string> fields;

    while (true)
    {
        if (current().type != token_t::symbol)
        {
            throw ParseError("Symbol as type field expected");
        }
        else {
            fields.push_back(current().data);
        }

        peek();

        auto term = require({token_t::right_paren, token_t::comma});

        if (term.type == token_t::comma)
        {
            continue;
        }
        else if (term.type == token_t::right_paren) {

            break;
        }
    }

    return boost::make_local_shared<ast::TypeDefinition>(name, std::move(fields));
}

std::vector<boost::local_shared_ptr<ast::Object>> Parser::resolve_function_arguments() noexcept(false)
{
    require({token_t::left_paren});

    if (match({token_t::right_paren}))
        return {};

    std::vector<boost::local_shared_ptr<ast::Object>> arguments;

    while (true)
    {
        arguments.push_back(primary());

        auto term = require({token_t::right_paren, token_t::comma});

        if (term.type == token_t::comma)
        {
            continue;
        }
        else if (term.type == token_t::right_paren) {

            break;
        }
    };

    return arguments;
}

boost::local_shared_ptr<ast::Object> Parser::resolve_array_subscript() noexcept(false)
{
    std::string symbol_name = previous().data;

    require({token_t::left_box_brace});

    auto term = primary();

    require({token_t::right_box_brace});

    return boost::make_local_shared<ast::ArraySubscriptOperator>(symbol_name, std::move(term));
}

boost::local_shared_ptr<ast::Object> Parser::resolve_symbol() noexcept(false)
{
    if (current().type == token_t::left_paren)
    {
        std::string data = previous().data;
        return boost::make_local_shared<ast::FunctionCall>(std::move(data), resolve_function_arguments());
    }
    else if (current().type == token_t::left_box_brace) {

        return resolve_array_subscript();
    }
    else {
        return binary(boost::make_local_shared<ast::Symbol>(previous().data));
    }
}
