#ifndef WEAK_AST_HPP
#define WEAK_AST_HPP

#include "../lexer/token.hpp"

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <boost/smart_ptr/make_local_shared.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace ast {

// clang-format off
enum struct ast_type_t { OBJECT, INTEGER, FLOAT, STRING, SYMBOL, ARRAY, ARRAY_SUBSCRIPT_OPERATOR, UNARY, BINARY, BLOCK, WHILE, FOR, IF, FUNCTION, FUNCTION_CALL, TYPE_CREATOR, TYPE_DEFINITION, TYPE_OBJECT, TYPE_FIELD };
// clang-format on

class Object {
public:
  virtual constexpr ast_type_t ast_type() const noexcept(true);
  virtual ~Object() = default;
};

class RootObject {
public:
  std::vector<boost::local_shared_ptr<Object>>& get() noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& get() const;
  void add(boost::local_shared_ptr<Object> expression) noexcept(false);

private:
  std::vector<boost::local_shared_ptr<Object>> expressions_;
};

class Integer : public Object {
public:
  Integer(std::string_view data) noexcept(false);
  Integer(size_t data) noexcept(true);
  size_t& value() noexcept(true);
  const size_t& value() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  size_t data_;
};

class Float : public Object {
public:
  Float(std::string_view data) noexcept(false);
  Float(double data) noexcept(true);
  double& value() noexcept(true);
  const double& value() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  double data_;
};

class String : public Object {
public:
  String(std::string data) noexcept(true);
  const std::string& value() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string data_;
};

class Symbol : public Object {
public:
  Symbol(std::string name) noexcept(true);
  const std::string& name() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
};

class Array : public Object {
public:
  Array(std::vector<boost::local_shared_ptr<Object>> elements) noexcept(true);
  std::vector<boost::local_shared_ptr<Object>>& elements() noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::vector<boost::local_shared_ptr<Object>> elements_;
};

class ArraySubscriptOperator : public Object {
public:
  ArraySubscriptOperator(std::string name, boost::local_shared_ptr<Object> index) noexcept(true);
  const std::string& symbol_name() const noexcept(true);
  const boost::local_shared_ptr<Object>& index() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
  boost::local_shared_ptr<Object> index_;
};

class Unary : public Object {
public:
  Unary(token_t type, boost::local_shared_ptr<Object> operation) noexcept(true);
  boost::local_shared_ptr<Object> operand() const noexcept(true);
  token_t type() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  token_t type_;
  boost::local_shared_ptr<Object> operation_;
};

class Binary : public Object {
public:
  Binary(token_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs) noexcept(true);
  const boost::local_shared_ptr<Object>& lhs() const noexcept(true);
  const boost::local_shared_ptr<Object>& rhs() const noexcept(true);
  token_t type() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  token_t type_;
  boost::local_shared_ptr<Object> lhs_;
  boost::local_shared_ptr<Object> rhs_;
};

class Block : public Object {
public:
  Block(std::vector<boost::local_shared_ptr<Object>> statements) noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& statements() const noexcept(true);
  std::vector<boost::local_shared_ptr<Object>>& statements() noexcept(false);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::vector<boost::local_shared_ptr<Object>> statements_;
};

class While : public Object {
public:
  While(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> block) noexcept(true);
  const boost::local_shared_ptr<Object>& exit_condition() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  boost::local_shared_ptr<Object> exit_condition_;
  boost::local_shared_ptr<Block> block_;
};

class For : public Object {
public:
  For() = default;

  void set_init(boost::local_shared_ptr<Object> init) noexcept(true);
  void set_exit_condition(boost::local_shared_ptr<Object> exit_condition) noexcept(true);
  void set_increment(boost::local_shared_ptr<Object> increment) noexcept(true);
  void set_body(boost::local_shared_ptr<Block> block) noexcept(true);
  const boost::local_shared_ptr<Object>& loop_init() const noexcept(true);
  const boost::local_shared_ptr<Object>& exit_condition() const noexcept(true);
  const boost::local_shared_ptr<Object>& increment() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  boost::local_shared_ptr<Object> init_;
  boost::local_shared_ptr<Object> exit_condition_;
  boost::local_shared_ptr<Object> increment_;
  boost::local_shared_ptr<Block> block_;
};

class If : public Object {
public:
  If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body) noexcept(true);
  If(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> body, boost::local_shared_ptr<Block> else_body) noexcept(true);
  const boost::local_shared_ptr<Object>& condition() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  const boost::local_shared_ptr<Block>& else_body() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  boost::local_shared_ptr<Object> exit_condition_;
  boost::local_shared_ptr<Block> body_;
  boost::local_shared_ptr<Block> else_body_;
};

class Function : public Object {
public:
  Function(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments, boost::local_shared_ptr<Block> body) noexcept(true);
  std::string name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
  boost::local_shared_ptr<Block> body_;
};

class FunctionCall : public Object {
public:
  FunctionCall(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
};

class TypeCreator : public Object {
public:
  TypeCreator(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
};

class TypeDefinition : public Object {
public:
  TypeDefinition(std::string_view name, std::vector<std::string> fields) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::vector<std::string>& fields() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string name_;
  std::vector<std::string> fields_;
};

class TypeObject : public Object {
public:
  TypeObject(std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>> arguments) noexcept(true);
  const std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>>& fields() const noexcept(false);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>> arguments_;
};

class TypeFieldOperator : public Object {
public:
  TypeFieldOperator(std::string type_name, std::string type_field) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::string& field() const noexcept(true);
  constexpr ast_type_t ast_type() const noexcept(true);

private:
  std::string type_name_;
  std::string type_field_;
};

constexpr ast_type_t Object::ast_type() const noexcept(true) {
  return ast_type_t::OBJECT;
}
constexpr ast_type_t Integer::ast_type() const noexcept(true) {
  return ast_type_t::INTEGER;
}
constexpr ast_type_t Float::ast_type() const noexcept(true) {
  return ast_type_t::FLOAT;
}
constexpr ast_type_t String::ast_type() const noexcept(true) {
  return ast_type_t::STRING;
}
constexpr ast_type_t Symbol::ast_type() const noexcept(true) {
  return ast_type_t::SYMBOL;
}
constexpr ast_type_t Array::ast_type() const noexcept(true) {
  return ast_type_t::ARRAY;
}
constexpr ast_type_t ArraySubscriptOperator::ast_type() const noexcept(true) {
  return ast_type_t::ARRAY_SUBSCRIPT_OPERATOR;
}
constexpr ast_type_t Unary::ast_type() const noexcept(true) {
  return ast_type_t::UNARY;
}
constexpr ast_type_t Binary::ast_type() const noexcept(true) {
  return ast_type_t::BINARY;
}
constexpr ast_type_t Block::ast_type() const noexcept(true) {
  return ast_type_t::BLOCK;
}
constexpr ast_type_t While::ast_type() const noexcept(true) {
  return ast_type_t::WHILE;
}
constexpr ast_type_t For::ast_type() const noexcept(true) {
  return ast_type_t::FOR;
}
constexpr ast_type_t If::ast_type() const noexcept(true) {
  return ast_type_t::IF;
}
constexpr ast_type_t Function::ast_type() const noexcept(true) {
  return ast_type_t::FUNCTION;
}
constexpr ast_type_t FunctionCall::ast_type() const noexcept(true) {
  return ast_type_t::FUNCTION_CALL;
}
constexpr ast_type_t TypeCreator::ast_type() const noexcept(true) {
  return ast_type_t::TYPE_CREATOR;
}
constexpr ast_type_t TypeDefinition::ast_type() const noexcept(true) {
  return ast_type_t::TYPE_DEFINITION;
}
constexpr ast_type_t TypeObject::ast_type() const noexcept(true) {
  return ast_type_t::TYPE_OBJECT;
}
constexpr ast_type_t TypeFieldOperator::ast_type() const noexcept(true) {
  return ast_type_t::TYPE_FIELD;
}

}// namespace ast

#endif// WEAK_AST_HPP
