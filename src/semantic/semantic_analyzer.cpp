#include "../../include/semantic/semantic_analyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<ast::RootObject> input)
    : m_input(input->get())
{ }

void SemanticAnalyzer::analyze()
{
    for (const auto& expression : m_input)
    {
        analyze_statement(expression);
    }
}

void SemanticAnalyzer::analyze_statement(std::shared_ptr<ast::Object> statement)
{
    if (const auto symbol_statement = std::dynamic_pointer_cast<ast::Symbol>(statement))
    {
        // ...
    }
    else if (const auto number_statement = std::dynamic_pointer_cast<ast::Number>(statement)) {

        // ...
    }
    else if (const auto string_statement = std::dynamic_pointer_cast<ast::String>(statement)) {

        // ...
    }
    else if (auto block_statement = std::dynamic_pointer_cast<ast::Block>(statement)) {

        analyze_block_statement(std::move(block_statement));
    }
    else if (auto if_statement = std::dynamic_pointer_cast<ast::If>(statement)) {

        analyze_if_statement(std::move(if_statement));
    }
    else if (auto while_statement = std::dynamic_pointer_cast<ast::While>(statement)) {

        analyze_while_statement(std::move(while_statement));
    }
    else if (auto for_statement = std::dynamic_pointer_cast<ast::For>(statement)) {

        analyze_for_statement(std::move(for_statement));
    }
    else if (auto function_statement = std::dynamic_pointer_cast<ast::Function>(statement)) {

        analyze_function_statement(std::move(function_statement));
    }
    else if (auto function_call_statement = std::dynamic_pointer_cast<ast::FunctionCall>(statement)) {

        analyze_function_call_statement(std::move(function_call_statement));
    }
    else if (auto binary_statement = std::dynamic_pointer_cast<ast::Binary>(statement)) {

        if (binary_statement->type() == lexeme_t::assign
            ||  binary_statement->type() == lexeme_t::plus_assign
            ||  binary_statement->type() == lexeme_t::minus_assign
            ||  binary_statement->type() == lexeme_t::star_assign
            ||  binary_statement->type() == lexeme_t::slash_assign)
        {
            analyze_assign_statement(std::move(binary_statement));
        }
        else {
            analyze_binary_statement(std::move(binary_statement));
        }
    }
    else {
        throw SemanticError("Unexpected statement");
    }
}

void SemanticAnalyzer::analyze_assign_statement(std::shared_ptr<ast::Binary> statement)
{
    if (!std::dynamic_pointer_cast<ast::Symbol>(statement->lhs()))
    {
        throw SemanticError("Expression is not assignable");
    }

    if (!std::dynamic_pointer_cast<ast::Symbol>(statement->rhs()) &&
        !std::dynamic_pointer_cast<ast::Number>(statement->rhs()) &&
        !std::dynamic_pointer_cast<ast::String>(statement->rhs()) &&
        !std::dynamic_pointer_cast<ast::Binary>(statement->rhs()) &&
        !std::dynamic_pointer_cast<ast::FunctionCall>(statement->rhs()))
    {
        throw SemanticError("Expression is not assignable");
    }

    if (auto binary = std::dynamic_pointer_cast<ast::Binary>(statement->rhs()))
    {
        analyze_binary_statement(binary);
    }
}

void SemanticAnalyzer::analyze_binary_statement(std::shared_ptr<ast::Binary> statement)
{
    switch (statement->type())
    {
        case lexeme_t::plus:
        case lexeme_t::minus:
        case lexeme_t::star:
        case lexeme_t::slash:
        case lexeme_t::remainder:
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

void SemanticAnalyzer::analyze_if_statement(std::shared_ptr<ast::If> if_statement)
{
    if (!to_bool_convertible(if_statement->condition()))
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

void SemanticAnalyzer::analyze_while_statement(std::shared_ptr<ast::While> while_statement)
{
    if (!to_bool_convertible(while_statement->exit_condition()))
    {
        throw SemanticError("While condition requires convertible to bool expression");
    }

    auto body = std::dynamic_pointer_cast<ast::Block>(while_statement->body());

    for (const auto& while_instruction : body->statements())
    {
        analyze_statement(while_instruction);
    }
}

void SemanticAnalyzer::analyze_for_statement(std::shared_ptr<ast::For> for_statement)
{
    if (for_statement->loop_init())
    {
        if (auto for_init = std::dynamic_pointer_cast<ast::Binary>(for_statement->loop_init()))
        {
            if (for_init->type() != lexeme_t::assign)
            {
                throw SemanticError("Bad for init");
            }
        }
        else {
            throw SemanticError("Bad for init");
        }
    }

    if (for_statement->exit_condition())
    {
        if (!to_bool_convertible(for_statement->exit_condition()))
        {
            throw SemanticError("For condition requires convertible to bool expression");
        }
    }

    if (for_statement->increment())
    {
        if (!std::dynamic_pointer_cast<ast::Unary>(for_statement->increment())
        &&  !std::dynamic_pointer_cast<ast::Binary>(for_statement->increment()))
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

void SemanticAnalyzer::analyze_function_call_statement(std::shared_ptr<ast::FunctionCall> function_statement)
{
    for (const auto& argument : function_statement->arguments())
    {
        if (!std::dynamic_pointer_cast<ast::Number>(argument)
            &&  !std::dynamic_pointer_cast<ast::String>(argument)
            &&  !std::dynamic_pointer_cast<ast::Symbol>(argument)
            &&  !std::dynamic_pointer_cast<ast::Binary>(argument)
            &&  !std::dynamic_pointer_cast<ast::FunctionCall>(argument))
        {
            throw SemanticError("Wrong function call argument");
        }
    }
}

void SemanticAnalyzer::analyze_function_statement(std::shared_ptr<ast::Function> function_statement)
{
    /// Function arguments are easily checked in parser.
    analyze_block_statement(function_statement->body());
}

void SemanticAnalyzer::analyze_block_statement(std::shared_ptr<ast::Block> block_statement)
{
    for (const auto& statement : block_statement->statements())
    {
        analyze_statement(statement);
    }
}

bool SemanticAnalyzer::to_bool_convertible(std::shared_ptr<ast::Object> statement)
{
    if (std::dynamic_pointer_cast<ast::Number>(statement))
    {
        return true;
    }
    else if (std::dynamic_pointer_cast<ast::Symbol>(statement)) {

        return true;
    }
    else if (auto binary_expression = std::dynamic_pointer_cast<ast::Binary>(statement)) {

        try
        {
            analyze_binary_statement(binary_expression);

        } catch (SemanticError&) {

            return false;
        }

        return true;
    }
    else {
        return false;
    }
}