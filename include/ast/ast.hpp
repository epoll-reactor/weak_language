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
enum struct type_t : uint32_t {
  OBJECT          = (1 << 1),
  INTEGER         = (1 << 2),
  FLOAT           = (1 << 3),
  STRING          = (1 << 4),
  SYMBOL          = (1 << 5),
  ARRAY           = (1 << 6),
  UNARY           = (1 << 7),
  BINARY          = (1 << 8),
  BLOCK           = (1 << 9),
  WHILE           = (1 << 10),
  FOR             = (1 << 11),
  IF              = (1 << 12),
  LAMBDA          = (1 << 13),
  LAMBDA_CALL     = (1 << 14),
  TYPE_CREATOR    = (1 << 15),
  TYPE_DEFINITION = (1 << 16),
  TYPE_OBJECT     = (1 << 17),
  TYPE_FIELD      = (1 << 18)
};
// clang-format on

constexpr uint32_t operator&(type_t lhs, type_t rhs) noexcept(true) {
  return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}
constexpr uint32_t operator|(type_t lhs, type_t rhs) noexcept(true) {
  return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

class Object {
public:
  virtual constexpr type_t ast_type() const noexcept(true);
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
  constexpr type_t ast_type() const noexcept(true) override;

private:
  size_t data_;
};

class Float : public Object {
public:
  Float(std::string_view data) noexcept(false);
  Float(double data) noexcept(true);
  double& value() noexcept(true);
  const double& value() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  double data_;
};

class String : public Object {
public:
  String(std::string data) noexcept(true);
  std::string& value() noexcept(true);
  const std::string& value() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string data_;
};

class Symbol : public Object {
public:
  Symbol(std::string name) noexcept(true);
  const std::string& name() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string name_;
};

class Array : public Object {
public:
  Array(std::vector<boost::local_shared_ptr<Object>> elements) noexcept(true);
  std::vector<boost::local_shared_ptr<Object>>& elements() noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::vector<boost::local_shared_ptr<Object>> elements_;
};

class Unary : public Object {
public:
  Unary(token_t type, boost::local_shared_ptr<Object> operation) noexcept(true);
  boost::local_shared_ptr<Object>& operand() noexcept(true);
  const boost::local_shared_ptr<Object>& operand() const noexcept(true);
  token_t type() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  token_t type_;
  boost::local_shared_ptr<Object> operation_;
};

class Binary : public Object {
public:
  Binary(token_t type, boost::local_shared_ptr<Object> lhs, boost::local_shared_ptr<Object> rhs) noexcept(true);
  boost::local_shared_ptr<Object>& lhs() noexcept(true);
  const boost::local_shared_ptr<Object>& lhs() const noexcept(true);
  boost::local_shared_ptr<Object>& rhs() noexcept(true);
  const boost::local_shared_ptr<Object>& rhs() const noexcept(true);
  token_t type() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

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
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::vector<boost::local_shared_ptr<Object>> statements_;
};

class While : public Object {
public:
  While(boost::local_shared_ptr<Object> exit_condition, boost::local_shared_ptr<Block> block) noexcept(true);
  const boost::local_shared_ptr<Object>& exit_condition() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

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
  constexpr type_t ast_type() const noexcept(true) override;

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
  constexpr type_t ast_type() const noexcept(true) override;

private:
  boost::local_shared_ptr<Object> exit_condition_;
  boost::local_shared_ptr<Block> body_;
  boost::local_shared_ptr<Block> else_body_;
};

class Lambda : public Object {
public:
  Lambda(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments, boost::local_shared_ptr<Block> body) noexcept(true);
  std::string name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  const boost::local_shared_ptr<Block>& body() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
  boost::local_shared_ptr<Block> body_;
};

class LambdaCall : public Object {
public:
  LambdaCall(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
};

class TypeCreator : public Object {
public:
  TypeCreator(std::string name, std::vector<boost::local_shared_ptr<Object>> arguments) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::vector<boost::local_shared_ptr<Object>>& arguments() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string name_;
  std::vector<boost::local_shared_ptr<Object>> arguments_;
};

class TypeDefinition : public Object {
public:
  TypeDefinition(std::string_view name, std::vector<std::string> fields) noexcept(true);
  const std::string& type_name() const noexcept(true);
  const std::vector<std::string>& names() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string type_name_;
  std::vector<std::string> names_;
};

class TypeObject : public Object {
public:
  TypeObject(std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>> arguments) noexcept(true);
  const std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>>& fields() const noexcept(false);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::vector<std::pair<std::string, boost::local_shared_ptr<Object>>> arguments_;
};

class TypeFieldOperator : public Object {
public:
  TypeFieldOperator(std::string type_name, std::string type_field) noexcept(true);
  const std::string& name() const noexcept(true);
  const std::string& field() const noexcept(true);
  constexpr type_t ast_type() const noexcept(true) override;

private:
  std::string type_name_;
  std::string type_field_;
};

constexpr type_t Object::ast_type() const noexcept(true) {
  return type_t::OBJECT;
}
constexpr type_t Integer::ast_type() const noexcept(true) {
  return type_t::INTEGER;
}
constexpr type_t Float::ast_type() const noexcept(true) {
  return type_t::FLOAT;
}
constexpr type_t String::ast_type() const noexcept(true) {
  return type_t::STRING;
}
constexpr type_t Symbol::ast_type() const noexcept(true) {
  return type_t::SYMBOL;
}
constexpr type_t Array::ast_type() const noexcept(true) {
  return type_t::ARRAY;
}
constexpr type_t Unary::ast_type() const noexcept(true) {
  return type_t::UNARY;
}
constexpr type_t Binary::ast_type() const noexcept(true) {
  return type_t::BINARY;
}
constexpr type_t Block::ast_type() const noexcept(true) {
  return type_t::BLOCK;
}
constexpr type_t While::ast_type() const noexcept(true) {
  return type_t::WHILE;
}
constexpr type_t For::ast_type() const noexcept(true) {
  return type_t::FOR;
}
constexpr type_t If::ast_type() const noexcept(true) {
  return type_t::IF;
}
constexpr type_t Lambda::ast_type() const noexcept(true) {
  return type_t::LAMBDA;
}
constexpr type_t LambdaCall::ast_type() const noexcept(true) {
  return type_t::LAMBDA_CALL;
}
constexpr type_t TypeCreator::ast_type() const noexcept(true) {
  return type_t::TYPE_CREATOR;
}
constexpr type_t TypeDefinition::ast_type() const noexcept(true) {
  return type_t::TYPE_DEFINITION;
}
constexpr type_t TypeObject::ast_type() const noexcept(true) {
  return type_t::TYPE_OBJECT;
}
constexpr type_t TypeFieldOperator::ast_type() const noexcept(true) {
  return type_t::TYPE_FIELD;
}

template <typename T>
struct remove_class {};
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...)> { using type = R(A...); };
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) const> { using type = R(A...); };
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) volatile> { using type = R(A...); };
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) const volatile> { using type = R(A...); };

template <typename T>
struct get_signature_impl { using type = typename remove_class<decltype(&std::remove_reference<T>::type::operator())>::type; };
template <typename R, typename... A>
struct get_signature_impl<R(A...)> { using type = R(A...); };
template <typename R, typename... A>
struct get_signature_impl<R (&)(A...)> { using type = R(A...); };
template <typename R, typename... A>
struct get_signature_impl<R (*)(A...)> { using type = R(A...); };
template <typename T>
using get_signature = typename get_signature_impl<T>::type;

template <typename Base, typename T>
bool dynamic_typecase_impl(Base* base, std::function<void(T*)> function) {
  if (T* ptr = dynamic_cast<T*>(base)) {
    function(ptr);
    return true;
  }
  return false;
}
/*!
 * Performs type dispatching using dynamic_cast.
 *
 * @param base          - base class pointer.
 * @param type_case     - functional object.
 * @param type_cases    - rest of functional objects.
 * @code
 *   Base* pointer = new Derived_3;
 *   dynamic_typecase(pointer,
 *     [](Derived_1* ptr) { ... },
 *     [](Derived_2* ptr) { ... },
 *     [](Derived_3* ptr) { ... }
 *   );
 * @endcode
 */
template <typename Base, typename TypeCase, typename... TypeCases>
void dynamic_typecase(Base* base, TypeCase&& type_case, TypeCases&&... type_cases) {
  using signature = get_signature<TypeCase>;
  if (dynamic_typecase_impl(base, static_cast<std::function<signature>>(type_case))) {
    return;
  }
  dynamic_typecase(base, type_cases...);
}
template <typename Base>
void dynamic_typecase(Base*) { /* Stop case */
}

}// namespace ast

#endif// WEAK_AST_HPP
