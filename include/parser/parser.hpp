#ifndef PARSER_HPP
#define PARSER_HPP

#include <optional>

#include <boost/pool/pool_alloc.hpp>

#include "../lexer/lexeme.hpp"
#include "../parser/ast.hpp"
#include "../parser/parse_error.hpp"


/// LL Syntax analyzer.
class Parser
{
public:
    explicit Parser(std::vector<Lexeme> lexemes);

    std::shared_ptr<ast::RootObject> parse();

private:
    /// @throws std::out_of_range
    const Lexeme& current() const;

    /// @throws std::out_of_range
    const Lexeme& previous() const;

    /// @throws std::out_of_range
    const Lexeme& peek();

    /// @return true if current lexeme is ';', ')' or ','
    bool end_of_expression() const noexcept;

    /// @note   don't throw because guarantees that valid index will be passed to at() method of m_input array
    /// @return true if peek() returns an meaningful token
    bool has_next() const noexcept;

    /// @return true if statement is raw block
    static bool is_block(const std::shared_ptr<ast::Object>& statement) noexcept;

    /// @return true if statement is block-based statement, but not block
    static bool is_block_statement(const std::shared_ptr<ast::Object>& statement) noexcept;

    /// @brief  get current token and match with one of samples
    /// @return correct lexeme, std::nullopt if input find no more tokens
    std::optional<Lexeme> match(const std::vector<lexeme_t>& expected_types);

    /// @throws ParseError if the match was unsuccessful
    /// @return correct lexeme of one of expected types
    Lexeme require(const std::vector<lexeme_t>& expected_types);

    /// @brief main parse function
    std::shared_ptr<ast::Object> primary();

    /// @return array parse tree
    std::shared_ptr<ast::Object> array();

    /// @return parse tree if ptr is additive operation, unchanged ptr otherwise
    std::shared_ptr<ast::Object> additive();

    /// @return parse tree if ptr is multiplicative operation, unchanged ptr otherwise
    std::shared_ptr<ast::Object> multiplicative();

    /// @note   this function does not check operation types
    /// @pre    previous() returns number or symbol lexeme
    /// @post   previous() returns first lexeme after parsed binary expression
    /// @return binary parse tree
    std::shared_ptr<ast::Object> binary(const std::shared_ptr<ast::Object>& ptr);

    /// @pre    previous() returns unary operator
    /// @post   previous() returns first lexeme after parsed unary expression
    std::shared_ptr<ast::Object> unary();

    /// @pre    previous() returns '{' lexeme
    /// @post   current() returns '}' lexeme
    /// @return block parse tree of recursively parsed expressions
    std::shared_ptr<ast::Block> block();

    /// @pre    previous() returns 'if' lexeme
    /// @post   previous() returns first lexeme after parsed if
    /// @return if parse tree with or without else block
    std::shared_ptr<ast::Object> if_statement();

    /// @pre    previous() returns 'while' lexeme
    /// @post   previous() returns first lexeme after parsed while
    /// @return while parse tree
    std::shared_ptr<ast::Object> while_statement();

    /// @note   any number of C-style for blocks (for ( 1 ; 2 ; 3 )) can be empty
    /// @pre    previous() returns 'for' lexeme
    /// @post   previous() returns first lexeme after parsed for
    /// @return for parse tree
    std::shared_ptr<ast::Object> for_statement();

    /// @pre    previous() returns 'fun' lexeme
    /// @post   previous() returns first lexeme after function declaration
    /// @return function parse tree that contains function name, argument list and body (block)
    std::shared_ptr<ast::Object> function_declare_statement();

    /// @pre    previous() returns 'define-type' lexeme
    /// @post   previous() returns first lexeme after type definition
    /// @return parsed type definition with only field names
    std::shared_ptr<ast::Object> define_type_statement();

    /// @pre    previous() returns '(' lexeme
    /// @post   previous() returns ')' lexeme
    /// @return correct function argument list
    std::vector<std::shared_ptr<ast::Object>> resolve_function_arguments();

    /// @pre    previous() returns symbol lexeme
    /// @post   previous() returns ')' lexeme if function call argument processed, symbol lexeme otherwise
    /// @return symbol object, function call object if '(' token placed after symbol
    std::shared_ptr<ast::Object> resolve_symbol();

    /// @pre    previous() returns symbol lexeme
    /// @post   previous() returns ']' lexeme
    /// @return array subscript parse tree
    std::shared_ptr<ast::Object> resolve_array_subscript();

    boost::pool_allocator<
        ast::Object,
        boost::default_user_allocator_new_delete,
        boost::details::pool::default_mutex,
        16,
        8192
    > m_pool_allocator;
    std::vector<Lexeme> m_input;
    std::size_t m_current_index;
};

#endif // PARSER_HPP