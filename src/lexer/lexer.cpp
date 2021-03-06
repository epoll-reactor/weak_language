#include "../../include/lexer/lexer.hpp"

#include "../../include/grammar.hpp"

#include <sstream>

Lexer::Lexer(std::istringstream data)
  : input_(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>())
  , keywords_(test_keywords)
  , operators_(test_operators) {}

char Lexer::current() const {
  return input_[current_index_];
}

char Lexer::previous() const {
  return input_[current_index_ - 1];
}

char Lexer::peek() {
  return input_[current_index_++];
}

bool Lexer::has_next() const noexcept {
  return current_index_ < input_.size();
}

bool Lexer::is_alphanumeric(char token) noexcept {
  return isalpha(token) || token == '_' || token == '?' || token == '-';
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  tokens.reserve(input_.size() / 4);

  while (has_next()) {
    peek();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    switch (previous()) {
      case ' ':
      case '\n':
      case '\r':
      case '\f':
      case '\v':
      case '\t':
        // Skip \s
        break;

      case '0' ... '9':
        tokens.emplace_back(process_digit());
        break;

      case '_':
      case 'a' ... 'z':
      case 'A' ... 'Z':
        tokens.emplace_back(process_symbol());
        break;

      case '\"':
        tokens.emplace_back(process_string_literal());
        break;

      default:
        if (std::find_if(operators_.begin(), operators_.end(), [this](const std::pair<std::string, token_t>& pair) {
              return pair.first[0] == previous();
            }) != operators_.end()) {
          tokens.emplace_back(process_operator());
        } else {
          throw LexicalError("Unknown symbol: {} ({})", previous(), std::to_string(static_cast<int>(previous())));
        }
    }
  }
#pragma GCC diagnostic pop

  tokens.emplace_back(Token{"", token_t::END_OF_DATA});

  tokens.shrink_to_fit();

  return tokens;
}

Token Lexer::process_digit() {
  std::string digit(1, previous());
  size_t dots_reached = 0;

  while (has_next() && (isdigit(current()) || current() == '.')) {
    if (current() == '.') {
      dots_reached++;
    }
    digit += peek();
  }

  if (has_next() && is_alphanumeric(current())) {
    throw LexicalError("Symbol can't start with digit");
  }
  if (dots_reached > 1) {
    throw LexicalError("Extra \".\" detected");
  }
  if (digit.back() == '.') {
    throw LexicalError("Digit after \".\" expected");
  }

  return Token{std::move(digit), (dots_reached == 0) ? token_t::NUM : token_t::FLOAT};
}

Token Lexer::process_string_literal() {
  peek();/// Eat opening "

  if (previous() == '\"') {
    return Token{"", token_t::STRING_LITERAL};
  }
  std::string literal(1, previous());
  while (current() != '\"') {
    if (has_next() && current() == '\\') {
      peek();
    }
    if (current() == '\0') {
      throw LexicalError("Closing '\\\"' expected");
    }
    literal += peek();
  }
  peek();/// Eat closing "

  return Token{std::move(literal), token_t::STRING_LITERAL};
}

Token Lexer::process_symbol() noexcept {
  std::string symbol(1, previous());
  while (has_next() && (is_alphanumeric(current()) || isdigit(current()))) {
    symbol += peek();
  }
  if (keywords_.find(symbol) != keywords_.end()) {
    return Token{"", keywords_.at(symbol)};
  } else {
    return Token{std::move(symbol), token_t::SYMBOL};
  }
}

Token Lexer::process_operator() {
  std::string op(1, previous());
  while (has_next() && operators_.find(op) != operators_.end()) {
    op += peek();
    if (operators_.find(op) == operators_.end()) {
      op.pop_back();
      --current_index_;
      break;
    }
  }
  const token_t type = operators_.at(op);

  return Token{"", type};
}
