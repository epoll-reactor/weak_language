#include "../../include/semantic/semantic_analyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer(boost::local_shared_ptr<ast::RootObject> input) noexcept(false)
{
    for (const auto& expr : input->get()) {
        m_input.emplace_back(expr);
    }
}

void SemanticAnalyzer::analyze() noexcept(false)
{
    for (const auto& expression : m_input) {
        analyze_statement(expression);
    }
}

void SemanticAnalyzer::analyze_statement(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false)
{
    ast::ast_type_t statement_type = statement->ast_type();

    if (statement_type == ast::ast_type_t::STRING
    ||  statement_type == ast::ast_type_t::INTEGER
    ||  statement_type == ast::ast_type_t::FLOAT
    ||  statement_type == ast::ast_type_t::SYMBOL) {
        return;
    }

    if (statement_type == ast::ast_type_t::BINARY) {
        auto binary = boost::static_pointer_cast<ast::Binary>(statement);

        token_t binary_type = binary->type();

        if (token_traits::is_assign_operator(binary_type)) {
            analyze_assign_statement(binary);
        }
        else {
            analyze_binary_statement(binary);
        }
        return;
    }

    if (statement_type == ast::ast_type_t::UNARY) {
        auto unary = boost::static_pointer_cast<ast::Unary>(statement);

        if (unary->operand()->ast_type() != ast::ast_type_t::INTEGER
        &&  unary->operand()->ast_type() != ast::ast_type_t::FLOAT
        &&  unary->operand()->ast_type() != ast::ast_type_t::SYMBOL) {
            throw SemanticError("Invalid unary operands");
        }
        return;
    }

    if (statement_type == ast::ast_type_t::BLOCK) {
        analyze_block_statement(boost::static_pointer_cast<ast::Block>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::TYPE_DEFINITION) {
        /* Actually nothing to analyze. */
        return;
    }
    if (statement_type == ast::ast_type_t::ARRAY) {
        analyze_array_statement(boost::static_pointer_cast<ast::Array>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR) {
        analyze_array_subscript_statement(boost::static_pointer_cast<ast::ArraySubscriptOperator>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::FUNCTION) {
        analyze_function_statement(boost::static_pointer_cast<ast::Function>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::FUNCTION_CALL) {
        analyze_function_call_statement(boost::static_pointer_cast<ast::FunctionCall>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::IF) {
        analyze_if_statement(boost::static_pointer_cast<ast::If>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::WHILE) {
        analyze_while_statement(boost::static_pointer_cast<ast::While>(statement));
        return;
    }
    if (statement_type == ast::ast_type_t::FOR) {
        analyze_for_statement(boost::static_pointer_cast<ast::For>(statement));
        return;
    }

    throw SemanticError("Unexpected statement");
}

void SemanticAnalyzer::analyze_array_statement(const boost::local_shared_ptr<ast::Array>& statement) noexcept(false)
{
    for (const auto& element : statement->elements()) {
        ast::ast_type_t element_type = element->ast_type();
        if (element_type != ast::ast_type_t::SYMBOL
        &&  element_type != ast::ast_type_t::INTEGER
        &&  element_type != ast::ast_type_t::FLOAT
        &&  element_type != ast::ast_type_t::STRING
        &&  element_type != ast::ast_type_t::BINARY
        &&  element_type != ast::ast_type_t::FUNCTION_CALL
        &&  element_type != ast::ast_type_t::ARRAY) {
            throw SemanticError("Array expects object don't statement");
        }
    }
}

void SemanticAnalyzer::analyze_assign_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false)
{
    if (statement->lhs()->ast_type() != ast::ast_type_t::SYMBOL) {
        throw SemanticError("Expression is not assignable");
    }

    ast::ast_type_t statement_type = statement->ast_type();

    if (statement_type != ast::ast_type_t::SYMBOL
    &&  statement_type != ast::ast_type_t::INTEGER
    &&  statement_type != ast::ast_type_t::FLOAT
    &&  statement_type != ast::ast_type_t::STRING
    &&  statement_type != ast::ast_type_t::BINARY
    &&  statement_type != ast::ast_type_t::UNARY
    &&  statement_type != ast::ast_type_t::FUNCTION_CALL
    &&  statement_type != ast::ast_type_t::ARRAY
    &&  statement_type != ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR) {
        throw SemanticError("Expression is not assignable");
    }

    if (statement->rhs()->ast_type() == ast::ast_type_t::BINARY) {
        analyze_statement(boost::static_pointer_cast<ast::Binary>(statement->rhs()));
    }
}

void SemanticAnalyzer::analyze_binary_statement(const boost::local_shared_ptr<ast::Binary>& statement) noexcept(false)
{
    if (!token_traits::is_binary(statement->type())) {
        throw SemanticError("Incorrect binary expression operator: " + dispatch_token(statement->type()));
    }

    if (auto lhs = boost::dynamic_pointer_cast<ast::Binary>(statement->lhs())) {
        analyze_binary_statement(lhs);
    }
    else if (auto rhs = boost::dynamic_pointer_cast<ast::Binary>(statement->rhs())) {
        analyze_binary_statement(rhs);
    }
}

void SemanticAnalyzer::analyze_function_call_statement(const boost::local_shared_ptr<ast::FunctionCall>& function_statement) noexcept(false)
{
    for (const auto& argument : function_statement->arguments()) {
        ast::ast_type_t argument_type = argument->ast_type();
        if (argument_type != ast::ast_type_t::INTEGER
        &&  argument_type != ast::ast_type_t::FLOAT
        &&  argument_type != ast::ast_type_t::STRING
        &&  argument_type != ast::ast_type_t::SYMBOL
        &&  argument_type != ast::ast_type_t::BINARY
        &&  argument_type != ast::ast_type_t::UNARY
        &&  argument_type != ast::ast_type_t::FUNCTION_CALL
        &&  argument_type != ast::ast_type_t::ARRAY
        &&  argument_type != ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR) {
            throw SemanticError("Wrong function call argument");
        }
    }
}

void SemanticAnalyzer::analyze_function_statement(const boost::local_shared_ptr<ast::Function>& function_statement) noexcept(false)
{
    /// Function arguments are easily checked in parser.
    analyze_block_statement(function_statement->body());
}

void SemanticAnalyzer::analyze_if_statement(const boost::local_shared_ptr<ast::If>& if_statement) noexcept(false)
{
    if (!to_integral_convertible(if_statement->condition())) {
        throw SemanticError("If condition requires convertible to bool expression");
    }

    auto if_body = boost::dynamic_pointer_cast<ast::Block>(if_statement->body());
    auto else_body = boost::dynamic_pointer_cast<ast::Block>(if_statement->else_body());

    for (const auto& if_instruction : if_body->statements()) {
        analyze_statement(if_instruction);
    }

    if (else_body) {
        for (const auto& if_instruction : else_body->statements()) {
            analyze_statement(if_instruction);
        }
    }
}

void SemanticAnalyzer::analyze_while_statement(const boost::local_shared_ptr<ast::While>& while_statement) noexcept(false)
{
    if (!to_number_convertible(while_statement->exit_condition())) {
        throw SemanticError("While condition requires convertible to bool expression");
    }

    auto body = boost::dynamic_pointer_cast<ast::Block>(while_statement->body());

    for (const auto& while_instruction : body->statements()) {
        analyze_statement(while_instruction);
    }
}

void SemanticAnalyzer::analyze_for_statement(const boost::local_shared_ptr<ast::For>& for_statement) noexcept(false)
{
    if (for_statement->loop_init()) {
        if (for_statement->loop_init()->ast_type() != ast::ast_type_t::BINARY) { throw SemanticError("Bad for init"); }

        auto for_init = boost::static_pointer_cast<ast::Binary>(for_statement->loop_init());

        if (for_init->type() != token_t::assign) { throw SemanticError("For init part requires assignment operation"); }
    }

    if (for_statement->exit_condition()) {
        if (!to_integral_convertible(for_statement->exit_condition())) {
            throw SemanticError("For condition requires convertible to bool expression");
        }
    }

    if (for_statement->increment()) {
        if (for_statement->increment()->ast_type() != ast::ast_type_t::UNARY
        &&  for_statement->increment()->ast_type() != ast::ast_type_t::BINARY) {
            throw SemanticError("Bad for increment part");
        }
    }

    for (const auto& for_instruction : for_statement->body()->statements()) {
        analyze_statement(for_instruction);
    }
}

void SemanticAnalyzer::analyze_array_subscript_statement(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& statement) noexcept(false)
{
    if (!to_integral_convertible(statement->index())) {
        throw SemanticError("Array subscript operator requires convertible to number expression");
    }
}

void SemanticAnalyzer::analyze_block_statement(const boost::local_shared_ptr<ast::Block>& block_statement) noexcept(false)
{
    for (const auto& statement : block_statement->statements()) {
        analyze_statement(statement);
    }
}

bool SemanticAnalyzer::to_integral_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false)
{
    if (statement->ast_type() == ast::ast_type_t::INTEGER) {
        return true;
    }
    else if (statement->ast_type() == ast::ast_type_t::SYMBOL) {
        return true;
    }
    else if (statement->ast_type() == ast::ast_type_t::FUNCTION_CALL) {
        return true;
    }
    else if (auto binary_expression = boost::dynamic_pointer_cast<ast::Binary>(statement)) {
        try {
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

bool SemanticAnalyzer::to_number_convertible(const boost::local_shared_ptr<ast::Object>& statement) noexcept(false)
{
    return to_integral_convertible(statement) || statement->ast_type() == ast::ast_type_t::FLOAT;
}
