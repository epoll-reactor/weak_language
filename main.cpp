#include <bits/stdc++.h>

#include <utility>


class SyntaxError : public std::exception
{
public:
    explicit SyntaxError(std::string_view error_message)
        : m_error(error_message.data())
    { }

    const char* what() const noexcept override
    {
        return m_error.what();
    }

private:
    std::runtime_error m_error;
};

class ArgumentError : public std::exception
{
public:
    explicit ArgumentError(std::string_view error_message)
            : m_error(error_message.data())
    { }

    const char* what() const noexcept override
    {
        return m_error.what();
    }

private:
    std::runtime_error m_error;
};


enum class lexeme_t : std::uint8_t
{
    end_of_string,

    object,
    number_literal,
    string_literal,
    null_type,
    function,
    symbol,

    key_string,
    key_integer,

    key_if,
    key_for,
    key_while,

    key_assign,                 // =
    key_negation,               // !

    key_less,                   // <
    key_greater,                // >

    key_open_curly_bracket,     // {
    key_close_curly_bracket,    // }

    key_open_square_bracket,    // [
    key_close_square_bracket,   // ]

    key_open_round_bracket,     // (
    key_close_round_bracket,    // )

    key_plus,                   // +
    key_minus,                  // -
    key_slash,                  // /
    key_star,                   // *
    key_dot,                    // .
    key_comma,                  // ,
    key_colon,                  // :
    key_semicolon,              // ;
};


class Object : public std::enable_shared_from_this<Object>
{
public:
    virtual std::shared_ptr<Object> evaluate() = 0;
};


class NumberObject final : public Object
{
public:
    explicit NumberObject(std::int32_t data)
        : m_data(data)
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return shared_from_this();
    }

    const std::int32_t& value() const noexcept
    {
        return m_data;
    }

private:
    std::int32_t m_data;
};


class StringLiteralObject final : public Object
{
public:
    explicit StringLiteralObject(std::string data)
        : m_data(std::move(data))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return shared_from_this();
    }

    const std::string& value() const noexcept
    {
        return m_data;
    }

private:
    std::string m_data;
};


class NullObject final : public Object
{
public:
    NullObject() = default;

    std::shared_ptr<Object> evaluate() override
    {
        return shared_from_this();
    }
};


class FunctionObject final : public Object
{
public:
    explicit FunctionObject(
        lexeme_t return_type,
        std::vector<std::shared_ptr<Object>> instructions,
        std::vector<std::shared_ptr<Object>> arguments)
        : m_return_type(return_type)
        , m_local_scope(std::move(instructions))
        , m_function_arguments(std::move(arguments))
    { }

    std::vector<std::shared_ptr<Object>>& instructions() noexcept
    {
        return m_local_scope;
    }

    std::vector<std::shared_ptr<Object>>& arguments() noexcept
    {
        return m_function_arguments;
    }

    lexeme_t return_type() noexcept
    {
        return m_return_type;
    }

    std::shared_ptr<Object> evaluate() override
    {
        return shared_from_this();
    }

private:
    lexeme_t m_return_type;
    std::vector<std::shared_ptr<Object>> m_local_scope;
    std::vector<std::shared_ptr<Object>> m_function_arguments;
};


class BinaryNumberOperation final : public Object
{
private:
    friend class BinaryAdditionObject;
    friend class BinarySubtractionObject;
    friend class BinaryMultiplicationObject;
    friend class BinaryDivisionObject;

    BinaryNumberOperation(std::string operation, std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
        , m_operation(std::move(operation))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        auto pre_computed_lhs = m_lhs->evaluate();
        auto pre_computed_rhs = m_rhs->evaluate();

        std::shared_ptr<NumberObject> lhs = std::dynamic_pointer_cast<NumberObject>(pre_computed_lhs);
        std::shared_ptr<NumberObject> rhs = std::dynamic_pointer_cast<NumberObject>(pre_computed_rhs);

        if (!lhs)
        {
            throw ArgumentError("Left argument not a number");
        }

        if (!rhs)
        {
            throw ArgumentError("Right argument not a number");
        }

        if (m_operator_lambdas.find(m_operation) != m_operator_lambdas.end())
        {
            return m_operator_lambdas[m_operation](lhs->value(), rhs->value());
        }
        else {
            throw ArgumentError("Unknown binary operator");
        }
    }

    using execution_policy_t = std::function<std::shared_ptr<Object>(std::int32_t lhs, std::int32_t rhs)>;

    std::shared_ptr<Object> m_lhs;
    std::shared_ptr<Object> m_rhs;
    std::string m_operation;

    std::unordered_map<std::string, execution_policy_t> m_operator_lambdas = {
        {"+", [](std::int32_t lhs, std::int32_t rhs) noexcept {
            return std::make_unique<NumberObject>(lhs + rhs);
        }},
        {"-", [](std::int32_t lhs, std::int32_t rhs) noexcept {
            return std::make_unique<NumberObject>(lhs - rhs);
        }},
        {"*", [](std::int32_t lhs, std::int32_t rhs) noexcept {
            return std::make_unique<NumberObject>(lhs * rhs);
        }},
        {"/", [](std::int32_t lhs, std::int32_t rhs) noexcept {
            return std::make_unique<NumberObject>(lhs / rhs);
        }}
    };
};


class BinaryAdditionObject final : public Object
{
public:
    BinaryAdditionObject(std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_common_operation("+", std::move(lhs), std::move(rhs))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return m_common_operation.evaluate();
    }

private:
    BinaryNumberOperation m_common_operation;
};


class BinarySubtractionObject final : public Object
{
public:
    BinarySubtractionObject(std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_common_operation("-", std::move(lhs), std::move(rhs))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return m_common_operation.evaluate();
    }

private:
    BinaryNumberOperation m_common_operation;
};


class BinaryMultiplicationObject final : public Object
{
public:
    BinaryMultiplicationObject(std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_common_operation("*", std::move(lhs), std::move(rhs))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return m_common_operation.evaluate();
    }

private:
    BinaryNumberOperation m_common_operation;
};


class BinaryDivisionObject final : public Object
{
public:
    BinaryDivisionObject(std::shared_ptr<Object> lhs, std::shared_ptr<Object> rhs)
        : m_common_operation("/", std::move(lhs), std::move(rhs))
    { }

    std::shared_ptr<Object> evaluate() override
    {
        return m_common_operation.evaluate();
    }

private:
    BinaryNumberOperation m_common_operation;
};


struct Lexeme
{
    lexeme_t type;
    std::optional<std::string> optional_data;
};


class Lexer
{
public:
    explicit Lexer(std::istringstream parse_data)
        : m_parse_data((std::istreambuf_iterator<char>(parse_data)), std::istreambuf_iterator<char>())
    { }

    std::vector<Lexeme> tokenize()
    {
        std::vector<Lexeme> lexemes;

        while (!m_parse_data.empty())
        {
            char token = ' ';

            while (isspace(token))
            {
                token = peek();
            }

            if (token == '\"')
            {
                token = peek(); /// Eat opening "
                lexemes.emplace_back(process_string_literal(token));
                token = peek(); /// Eat closing "
            }

            if (isdigit(token)) /// Digits must be parsed before operators
            {
                lexemes.emplace_back(process_digit(token));
            }

            if (is_alphanumeric(token))
            {
                const Lexeme symbol = process_symbol(token);
                const std::string& value = symbol.optional_data.value();

                if (contains(m_keyword_lexemes, value))
                {
                    lexemes.emplace_back(Lexeme{m_keyword_lexemes[value], std::nullopt});
                }
                else {
                    lexemes.emplace_back(symbol);
                }
            }

            if (contains(m_operator_lexemes, token))
            {
                lexemes.emplace_back(Lexeme{m_operator_lexemes[token], std::nullopt});
            }
        }

        lexemes.emplace_back(Lexeme{lexeme_t::end_of_string, std::nullopt});

        return lexemes;
    }

private:
    template <typename MapContainer, typename String>
    bool contains(MapContainer&& container, String&& key)
    {
        return container.find(key) != container.end();
    }

    char peek()
    {
        char top_char = m_parse_data.front();
        m_parse_data.pop_front();

        return top_char;
    }

    static bool is_alphanumeric(char token)
    {
        return isalpha(token) || token == '_';
    }

    Lexeme process_digit(char& token)
    {
        std::string number;
        char previous_char = '\0';
        std::size_t dots = 0;

        while (isdigit(token) || token == '.')
        {
            if (token == '.')
            {
                ++dots;
            }

            number += token;
            previous_char = token;
            token = peek();
        }

        if (is_alphanumeric(token)) /// `000a` is neither numeric nor string literal
        {
            throw SyntaxError("Symbol can't start with digit");
        }

        if (dots > 1)
        {
            throw SyntaxError("Extra \".\" detected in number literal");
        }

        if (previous_char != '.')
        {
            return Lexeme{lexeme_t::number_literal, std::move(number)};
        }
        else {
            throw SyntaxError("Unexpected \".\"");
        }
    }

    Lexeme process_symbol(char& token)
    {
        std::string symbol;

        while (is_alphanumeric(token) || isdigit(token))
        {
            symbol += token;
            token = peek();
        }

        return Lexeme{lexeme_t::symbol, (symbol)};
    }

    Lexeme process_string_literal(char& token)
    {
        std::string symbol;
        char previous_char = '\0';

        while (token != '\"')
        {
            if (token == '\\')
            {
                token = peek(); /// Eat next character after '\'
            }

            if (token == '\0')
            {
                throw SyntaxError("Expected closing '\"'");
            }

            symbol += token;
            token = peek();
        }

        return Lexeme{lexeme_t::string_literal, std::move(symbol)};
    }

    std::unordered_map<std::string, lexeme_t> m_keyword_lexemes = {
        {"string",  lexeme_t::key_string},
        {"integer", lexeme_t::key_integer},
        {"if",      lexeme_t::key_if},
        {"for",     lexeme_t::key_for},
        {"while",   lexeme_t::key_while}
    };

    std::unordered_map<char, lexeme_t> m_operator_lexemes = {
        {'=', lexeme_t::key_assign},
        {'!', lexeme_t::key_negation},

        {';', lexeme_t::key_semicolon},
        {':', lexeme_t::key_colon},
        {'>', lexeme_t::key_greater},
        {'<', lexeme_t::key_less},

        {'+', lexeme_t::key_plus},
        {'-', lexeme_t::key_minus},
        {'*', lexeme_t::key_star},
        {'/', lexeme_t::key_slash},
        {'.', lexeme_t::key_dot},
        {',', lexeme_t::key_comma},
        {':', lexeme_t::key_colon},
        {';', lexeme_t::key_semicolon},

        {'{', lexeme_t::key_open_curly_bracket},
        {'}', lexeme_t::key_close_curly_bracket},
        {'[', lexeme_t::key_open_square_bracket},
        {']', lexeme_t::key_close_square_bracket},
        {'(', lexeme_t::key_open_round_bracket},
        {')', lexeme_t::key_close_round_bracket},
    };

    std::deque<char> m_parse_data;
};

class Parser
{
public:
    explicit Parser(std::vector<Lexeme> lexemes)
        : m_lexemes(std::move(lexemes))
    { }

private:
    std::vector<Lexeme> m_lexemes;
};


void test_casts()
{
    std::shared_ptr<Object> function_ptr(std::make_shared<FunctionObject>(
        lexeme_t::null_type,
        std::initializer_list<std::shared_ptr<Object>>{
            std::make_shared<StringLiteralObject>("String")
        },
        std::initializer_list<std::shared_ptr<Object>>{
            std::make_shared<StringLiteralObject>("String"),
            std::make_shared<StringLiteralObject>("String 2")
        }
    ));

    assert(std::dynamic_pointer_cast<FunctionObject>(function_ptr));
    assert(not std::dynamic_pointer_cast<StringLiteralObject>(function_ptr));
    assert(not std::dynamic_pointer_cast<NumberObject>(function_ptr));

    assert(std::dynamic_pointer_cast<FunctionObject>(function_ptr)->return_type() == lexeme_t::null_type);
    assert(std::make_shared<StringLiteralObject>("String")->value() ==
        std::dynamic_pointer_cast<StringLiteralObject>(
            std::dynamic_pointer_cast<FunctionObject>(function_ptr)->instructions()[0])->value());

    std::vector<std::string> test_values = {"String", "String 2"};
    for (std::size_t i = 0; i < test_values.size(); i++)
    {
        assert(std::make_shared<StringLiteralObject>(test_values[i])->value() ==
            std::dynamic_pointer_cast<StringLiteralObject>(
                std::dynamic_pointer_cast<FunctionObject>(function_ptr)->arguments()[i])->value());
    }
}

void test_evaluate_add_numbers()
{
    auto argument =
        std::make_shared<BinaryAdditionObject>(
        std::make_shared<NumberObject>(10),
        std::make_shared<NumberObject>(20));

    std::shared_ptr<NumberObject> value =
        std::dynamic_pointer_cast<NumberObject>(BinaryAdditionObject(
            argument, argument).evaluate());

    assert(value->value() == 60);
}

void test_evaluate_sub_numbers()
{
    auto argument =
        std::make_shared<BinarySubtractionObject>(
            std::make_shared<NumberObject>(10),
            std::make_shared<NumberObject>(20));

    std::shared_ptr<NumberObject> value =
        std::dynamic_pointer_cast<NumberObject>(BinarySubtractionObject(
            argument, argument).evaluate());

    assert(value->value() == 0);
}

void test_evaluate_mul_numbers()
{
    auto argument =
        std::make_shared<BinaryMultiplicationObject>(
            std::make_shared<NumberObject>(10),
            std::make_shared<NumberObject>(20));

    std::shared_ptr<NumberObject> value =
        std::dynamic_pointer_cast<NumberObject>(BinaryMultiplicationObject(
            argument, argument).evaluate());

    assert(value->value() == 40000);
}

void test_evaluate_div_numbers()
{
    auto argument1 =
        std::make_shared<BinaryDivisionObject>(
        std::make_shared<NumberObject>(20),
        std::make_shared<NumberObject>(2));

    auto argument2 =
        std::make_shared<BinaryDivisionObject>(
        std::make_shared<NumberObject>(10),
        std::make_shared<NumberObject>(2));

    std::shared_ptr<NumberObject> value =
        std::dynamic_pointer_cast<NumberObject>(BinaryDivisionObject(
        argument1, argument2).evaluate());

    assert(value->value() == 2);
}

void test_bad_evaluate_numbers()
{
    auto argument =
        std::make_shared<BinaryAdditionObject>(
        std::make_shared<NumberObject>(10),
        std::make_shared<StringLiteralObject>("Text"));

    try {
        std::shared_ptr<NumberObject> value =
            std::dynamic_pointer_cast<NumberObject>(BinaryAdditionObject(
                argument, argument).evaluate());
    } catch (ArgumentError& error) {
        return;
    }

    assert(false);
}

void debug_print_lexemes(const std::vector<Lexeme>& lexemes)
{
    for (const auto& lexeme : lexemes)
    {
        switch (lexeme.type)
        {
            case lexeme_t::object:
                std::cout << "Object: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::function:
                std::cout << "Function: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::number_literal:
                std::cout << "Number literal: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::string_literal:
                std::cout << "String literal: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::symbol:
                std::cout << "Symbol: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::key_if:
                std::cout << "If: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::key_for:
                std::cout << "For: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::key_while:
                std::cout << "While: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::key_string:
                std::cout << "String: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::key_integer:
                std::cout << "Integer: " << lexeme.optional_data.value() << "\n";
                break;

            case lexeme_t::null_type:
                std::cout << "Null type\n";
                break;

            case lexeme_t::key_close_curly_bracket:
                std::cout << "Close curly }\n";
                break;

            case lexeme_t::key_open_curly_bracket:
                std::cout << "Open curly {\n";
                break;

            case lexeme_t::key_close_round_bracket:
                std::cout << "Close round )\n";
                break;

            case lexeme_t::key_open_round_bracket:
                std::cout << "Open round (\n";
                break;

            case lexeme_t::key_open_square_bracket:
                std::cout << "Open square [\n";
                break;

            case lexeme_t::key_close_square_bracket:
                std::cout << "Close square ]\n";
                break;

            case lexeme_t::key_colon:
                std::cout << "Colon :\n";
                break;

            case lexeme_t::key_semicolon:
                std::cout << "Semicolon ;\n";
                break;

            case lexeme_t::key_assign:
                std::cout << "Assign =\n";
                break;

            case lexeme_t::key_negation:
                std::cout << "Negation !\n";
                break;

            case lexeme_t::key_less:
                std::cout << "Less <\n";
                break;

            case lexeme_t::key_greater:
                std::cout << "Greater >\n";
                break;

            case lexeme_t::key_plus:
                std::cout << "Plus +\n";
                break;

            case lexeme_t::key_minus:
                std::cout << "Minus -\n";
                break;

            case lexeme_t::end_of_string:
                std::cout << "EOF\n";
                break;
        }
    }
}

void test_lexemes(const std::vector<Lexeme>& lexemes, const std::vector<Lexeme>& control_lexemes)
{
    assert(lexemes.size() == control_lexemes.size());

    for (std::size_t i = 0; i < lexemes.size(); i++)
    {
        assert(lexemes[i].type == control_lexemes[i].type);
        assert(lexemes[i].optional_data == control_lexemes[i].optional_data);
    }
}

int main()
{
    test_casts();
    test_evaluate_div_numbers();
    test_evaluate_mul_numbers();
    test_evaluate_sub_numbers();
    test_evaluate_add_numbers();
    test_bad_evaluate_numbers();

    std::string payload = R"((((2 + 1) + 10 + 20) + 30 + 40))";

    std::istringstream data(payload);

    Lexer lexer(std::move(data));

    std::vector<Lexeme> lexemes = lexer.tokenize();

    debug_print_lexemes(lexemes);

    Parser parser(std::move(lexemes));
}
