#include "../../include/semantic/semantic_analyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer(const std::shared_ptr<ast::RootObject>& input)
    : m_input(input->get())
{ }

void SemanticAnalyzer::analyze()
{
    for (const auto& expression : m_input)
    {
        analyze_statement(expression);
    }
}

void SemanticAnalyzer::analyze_statement(const std::shared_ptr<ast::Object>& statement)
{
    if (statement->ast_type() == ast::ast_type_t::STRING
    ||  statement->ast_type() == ast::ast_type_t::INTEGER
    ||  statement->ast_type() == ast::ast_type_t::FLOAT
    ||  statement->ast_type() == ast::ast_type_t::SYMBOL)
        return;

    if (statement->ast_type() == ast::ast_type_t::BINARY)
    {
        auto binary = std::static_pointer_cast<ast::Binary>(statement);

        if (binary->type() == lexeme_t::assign
        ||  binary->type() == lexeme_t::plus_assign
        ||  binary->type() == lexeme_t::minus_assign
        ||  binary->type() == lexeme_t::star_assign
        ||  binary->type() == lexeme_t::slash_assign)
        {
            analyze_assign_statement(binary);
        }
        else {
            analyze_binary_statement(binary);
        }
        return;
    }

    if (statement->ast_type() == ast::ast_type_t::BLOCK) {
        analyze_block_statement(std::static_pointer_cast<ast::Block>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::TYPE_DEFINITION) {
        /* Actually nothing to analyze. */
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::ARRAY) {
        analyze_array_statement(std::static_pointer_cast<ast::Array>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR) {
        analyze_array_subscript_statement(std::static_pointer_cast<ast::ArraySubscriptOperator>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::FUNCTION) {
        analyze_function_statement(std::static_pointer_cast<ast::Function>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::FUNCTION_CALL) {
        analyze_function_call_statement(std::static_pointer_cast<ast::FunctionCall>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::IF) {
        analyze_if_statement(std::static_pointer_cast<ast::If>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::WHILE) {
        analyze_while_statement(std::static_pointer_cast<ast::While>(statement));
        return;
    }
    if (statement->ast_type() == ast::ast_type_t::FOR) {
        analyze_for_statement(std::static_pointer_cast<ast::For>(statement));
        return;
    }

    throw SemanticError("Unexpected statement");
}

void SemanticAnalyzer::analyze_array_statement(const std::shared_ptr<ast::Array>& statement)
{
    for (const auto& element : statement->elements())
    {
        if (element->ast_type() != ast::ast_type_t::SYMBOL
        &&  element->ast_type() != ast::ast_type_t::INTEGER
        &&  element->ast_type() != ast::ast_type_t::FLOAT
        &&  element->ast_type() != ast::ast_type_t::STRING
        &&  element->ast_type() != ast::ast_type_t::BINARY
        &&  element->ast_type() != ast::ast_type_t::FUNCTION_CALL
        &&  element->ast_type() != ast::ast_type_t::ARRAY)
        {
            throw SemanticError("Array expects object don't statement");
        }
    }
}

void SemanticAnalyzer::analyze_assign_statement(const std::shared_ptr<ast::Binary>& statement)
{
    if (statement->lhs()->ast_type() != ast::ast_type_t::SYMBOL)
    {
        throw SemanticError("Expression is not assignable");
    }

    if (statement->ast_type() != ast::ast_type_t::SYMBOL
    &&  statement->ast_type() != ast::ast_type_t::INTEGER
    &&  statement->ast_type() != ast::ast_type_t::FLOAT
    &&  statement->ast_type() != ast::ast_type_t::STRING
    &&  statement->ast_type() != ast::ast_type_t::BINARY
    &&  statement->ast_type() != ast::ast_type_t::FUNCTION_CALL
    &&  statement->ast_type() != ast::ast_type_t::ARRAY
    &&  statement->ast_type() != ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR)
    {
        throw SemanticError("Expression is not assignable");
    }

    if (statement->rhs()->ast_type() == ast::ast_type_t::BINARY)
    {
        analyze_statement(std::static_pointer_cast<ast::Binary>(statement->rhs()));
    }
}

void SemanticAnalyzer::analyze_binary_statement(const std::shared_ptr<ast::Binary>& statement)
{
    switch (statement->type())
    {
        case lexeme_t::plus:
        case lexeme_t::minus:
        case lexeme_t::star:
        case lexeme_t::slash:
        case lexeme_t::mod:
        case lexeme_t::eq:
        case lexeme_t::neq:
        case lexeme_t::gt:
        case lexeme_t::ge:
        case lexeme_t::lt:
        case lexeme_t::le:
            break;

        default:
            throw SemanticError("Incorrect binary expression operator");
    }

    if (auto lhs = std::dynamic_pointer_cast<ast::Binary>(statement->lhs()))
    {
        analyze_binary_statement(lhs);
    }
    else if (auto rhs = std::dynamic_pointer_cast<ast::Binary>(statement->rhs())) {

        analyze_binary_statement(rhs);
    }
}

void SemanticAnalyzer::analyze_function_call_statement(const std::shared_ptr<ast::FunctionCall>& function_statement)
{
    for (const auto& argument : function_statement->arguments())
    {
        if (argument->ast_type() != ast::ast_type_t::INTEGER
        &&  argument->ast_type() != ast::ast_type_t::FLOAT
        &&  argument->ast_type() != ast::ast_type_t::STRING
        &&  argument->ast_type() != ast::ast_type_t::SYMBOL
        &&  argument->ast_type() != ast::ast_type_t::BINARY
        &&  argument->ast_type() != ast::ast_type_t::FUNCTION_CALL
        &&  argument->ast_type() != ast::ast_type_t::ARRAY
        &&  argument->ast_type() != ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR)
        {
            throw SemanticError("Wrong function call argument");
        }
    }
}

void SemanticAnalyzer::analyze_function_statement(const std::shared_ptr<ast::Function>& function_statement)
{
    /// Function arguments are easily checked in parser.
    analyze_block_statement(function_statement->body());
}

void SemanticAnalyzer::analyze_if_statement(const std::shared_ptr<ast::If>& if_statement)
{
    if (!to_integral_convertible(if_statement->condition()))
    {
        throw SemanticError("If condition requires convertible to bool expression");
    }

    auto if_body = std::dynamic_pointer_cast<ast::Block>(if_statement->body());

    auto else_body = std::dynamic_pointer_cast<ast::Block>(if_statement->else_body());

    for (const auto& if_instruction : if_body->statements())
    {
        analyze_statement(if_instruction);
    }

    if (else_body)
    {
        for (const auto& if_instruction : else_body->statements())
        {
            analyze_statement(if_instruction);
        }
    }
}

void SemanticAnalyzer::analyze_while_statement(const std::shared_ptr<ast::While>& while_statement)
{
    if (!to_integral_convertible(while_statement->exit_condition()))
    {
        throw SemanticError("While condition requires convertible to bool expression");
    }

    auto body = std::dynamic_pointer_cast<ast::Block>(while_statement->body());

    for (const auto& while_instruction : body->statements())
    {
        analyze_statement(while_instruction);
    }
}

void SemanticAnalyzer::analyze_for_statement(const std::shared_ptr<ast::For>& for_statement)
{
    if (for_statement->loop_init())
    {
        if (auto for_init = std::dynamic_pointer_cast<ast::Binary>(for_statement->loop_init()))
        {
            if (for_init->type() != lexeme_t::assign)
            {
                throw SemanticError("For init part requires assignment operation");
            }
        }
        else {
            throw SemanticError("Bad for init");
        }
    }

    if (for_statement->exit_condition())
    {
        if (!to_integral_convertible(for_statement->exit_condition()))
        {
            throw SemanticError("For condition requires convertible to bool expression");
        }
    }

    if (for_statement->increment())
    {
        if (for_statement->increment()->ast_type() != ast::ast_type_t::UNARY
        &&  for_statement->increment()->ast_type() != ast::ast_type_t::BINARY)
        {
            throw SemanticError("Bad for increment part");
        }
    }

    auto body = for_statement->body();

    for (const auto& for_instruction : body->statements())
    {
        analyze_statement(for_instruction);
    }
}

void SemanticAnalyzer::analyze_array_subscript_statement(const std::shared_ptr<ast::ArraySubscriptOperator>& statement)
{
    if (!to_integral_convertible(statement->index()))
    {
        throw SemanticError("Array subscript operator requires convertible to number expression");
    }
}

void SemanticAnalyzer::analyze_block_statement(const std::shared_ptr<ast::Block>& block_statement)
{
    for (const auto& statement : block_statement->statements())
    {
        analyze_statement(statement);
    }
}

bool SemanticAnalyzer::to_integral_convertible(const std::shared_ptr<ast::Object>& statement)
{
    if (std::dynamic_pointer_cast<ast::Integer>(statement))
    {
        return true;
    }
    else if (std::dynamic_pointer_cast<ast::Symbol>(statement)) {

        return true;
    }
    else if (std::dynamic_pointer_cast<ast::FunctionCall>(statement)) {

        return true;
    }
    else if (auto binary_expression = std::dynamic_pointer_cast<ast::Binary>(statement)) {

        try
        {
            analyze_binary_statement(binary_expression);

        } catch (SemanticError& err) {

            return false;
        }

        return true;
    }
    else {
        return false;
    }
}

bool SemanticAnalyzer::to_number_convertible(const std::shared_ptr<ast::Object>& statement)
{
    return to_integral_convertible(statement) || statement->ast_type() == ast::ast_type_t::FLOAT;
}
