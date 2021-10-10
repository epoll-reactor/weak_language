#include "../../include/eval/eval.hpp"

#include "../../include/eval/implementation/binary.hpp"
#include "../../include/eval/implementation/unary.hpp"
#include "../../include/std/builtins.hpp"

#include <numeric>

#include <boost/range/combine.hpp>

ALWAYS_INLINE static constexpr bool is_datatype(const ast::Object* object) noexcept(true) {
  if (!object) {
    return false;
  }
  switch (object->ast_type()) {
    case ast::type_t::INTEGER:
    case ast::type_t::FLOAT:
    case ast::type_t::STRING:
    case ast::type_t::ARRAY:
    case ast::type_t::TYPE_OBJECT:
      return true;
    default:
      return false;
  }
}

ALWAYS_INLINE static void do_typecheck(const ast::Object* object, ast::type_t expected, const char* error_message) noexcept(false) {
  if (object->ast_type() != expected) {
    throw EvalError(error_message);
  }
}

ALWAYS_INLINE static void do_typecheck(ast::type_t target, ast::type_t expected, const char* error_message) noexcept(false) {
  if (target != expected) {
    throw EvalError(error_message);
  }
}
/// For some reason this function works incorrect with ALWAYS_INLINE specifier
static bool add_function(const boost::local_shared_ptr<ast::Object>& object, Storage& storage) noexcept(false) {
  if (const auto function = boost::dynamic_pointer_cast<ast::Lambda>(object)) {
    storage.push(function->name().data(), function);
    return true;
  }
  return false;
}

Evaluator::Evaluator(const boost::local_shared_ptr<ast::RootObject>& program) noexcept(false) {
  for (const auto& stmt : program->get()) {
    expressions_.emplace_back(stmt);
  }
}

void Evaluator::eval() noexcept(false) {
  for (const auto& expr : expressions_) {
    if (add_function(expr, storage_)) {
      continue;
    }
    add_type_definition(boost::dynamic_pointer_cast<ast::TypeDefinition>(expr));
  }
  call_function("main", {});
}

void Evaluator::add_type_definition(const boost::local_shared_ptr<ast::TypeDefinition>& definition) noexcept(false) {
  type_creators_.emplace(definition->type_name(), [&definition](const std::vector<boost::local_shared_ptr<ast::Object>>& names) {
    const auto& type_names = definition->names();
    if (type_names.size() != names.size()) {
      throw EvalError("new {}: wrong arguments size", definition->type_name());
    }
    std::vector<std::pair<std::string, boost::local_shared_ptr<ast::Object>>> arguments;
    for (const auto& pair : boost::combine(type_names, names)) {
      std::string type_name;
      boost::local_shared_ptr<ast::Object> name;
      boost::tie(type_name, name) = pair;
      arguments.emplace_back(std::move(type_name), std::move(name));
    }
    return boost::make_local_shared<ast::TypeObject>(std::move(arguments));
  });
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_type_creation_function(const boost::local_shared_ptr<ast::TypeCreator>& type_creator) noexcept(false) {
  return type_creators_[type_creator->name()](type_creator->arguments());
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_type_field_access(const boost::local_shared_ptr<ast::TypeFieldOperator>& type_field) noexcept(false) {
  const auto& name = type_field->name();
  const auto& field = type_field->field();
  const auto& object = storage_.lookup(name);

  if (object->ast_type() != ast::type_t::TYPE_OBJECT) {
    throw EvalError("Type object expected");
  }
  const auto& fields = boost::static_pointer_cast<ast::TypeObject>(object)->fields();
  auto found = std::find_if(fields.begin(), fields.end(), [&field](auto&& element) {
    return element.first == field;
  });
  if (found != fields.end()) {
    return found->second;
  } else {
    throw EvalError("{}: field not found - {}", name, field);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::call_function(std::string_view name, std::vector<boost::local_shared_ptr<ast::Object>> arguments) noexcept(false) {
  auto find_function = [this, &name] {
    const auto& stored_function = storage_.lookup(name.data()).get();
    do_typecheck(stored_function, ast::type_t::LAMBDA, "Try to call not a function");
    return static_cast<ast::Lambda*>(stored_function);
  };

  const auto function = find_function();
  const auto& args = function->arguments();
  const auto& body = function->body()->statements();

  if (body.empty()) {
    return {};
  }
  if (args.size() != arguments.size()) {
    throw EvalError("Wrong arguments size");
  }
  storage_.scope_begin();
  for (size_t i = 0; i < args.size(); ++i) {
    const std::string& argument_name = static_cast<ast::Symbol*>(args[i].get())->name();
    storage_.push(argument_name, arguments[i]);
  }
  if (UNLIKELY(body.size() == 1)) {
    auto last_statement = eval(body.front());
    if (is_datatype(last_statement.get())) {
      storage_.scope_end();
      return last_statement;
    }
  } else if (!body.empty()) {
    for (size_t i = 0; i < body.size() - 1; ++i) {
      eval(body[i]);
    }
    auto last_statement = eval(*--body.cend());
    if (is_datatype(last_statement.get())) {
      storage_.scope_end();
      return last_statement;
    }
  }
  storage_.scope_end();
  return {};
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_function_call(const boost::local_shared_ptr<ast::LambdaCall>& function_call) noexcept(false) {
  auto arguments = function_call->arguments();
  for (auto& argument : arguments) {
    auto type = argument->ast_type();
    if (type != ast::type_t::INTEGER && type != ast::type_t::FLOAT && type != ast::type_t::STRING) {
      argument = eval(argument);
    }
  }
  const std::string& name = function_call->name();
  if (builtins.contains(name)) {
    if (const auto result = builtins.at(name)(arguments)) {
      return result.value();
    } else {
      return nullptr;
    }
  }
  return call_function(name, arguments);
}

void Evaluator::eval_block(const boost::local_shared_ptr<ast::Block>& block) noexcept(false) {
  for (const auto& statement : block->statements()) {
    eval(statement);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_binary(const boost::local_shared_ptr<ast::Binary>& binary) noexcept(false) {
  const auto type = binary->type();

  if (type == token_t::assign) {
    const auto variable = boost::static_pointer_cast<ast::Symbol>(binary->lhs());
    storage_.overwrite(variable->name(), eval(binary->rhs()));
    return binary;
  }
  if (token_traits::is_assign_operator(type)) {
    const auto variable = boost::static_pointer_cast<ast::Symbol>(binary->lhs());
    storage_.push(
        variable->name(),
        eval_context::assign_binary_implementation(type, storage_.lookup(variable->name()), eval(binary->rhs())));
    return variable;
  }

  const auto lhs = eval(binary->lhs());
  const auto rhs = eval(binary->rhs());

  const ast::type_t lhs_t = lhs->ast_type();
  const ast::type_t rhs_t = rhs->ast_type();

  using ast_t = ast::type_t;
  if (lhs_t == ast_t::INTEGER && rhs_t == ast_t::INTEGER) {
    return eval_context::binary_implementation<ast::Integer, ast::Integer>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::INTEGER && rhs_t == ast_t::FLOAT) {
    return eval_context::binary_implementation<ast::Integer, ast::Float>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::FLOAT && rhs_t == ast_t::INTEGER) {
    return eval_context::binary_implementation<ast::Float, ast::Integer>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::FLOAT && rhs_t == ast_t::FLOAT) {
    return eval_context::binary_implementation<ast::Float, ast::Float>(type, lhs, rhs);
  }
  throw EvalError("Unknown binary expr");
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_unary(const boost::local_shared_ptr<ast::Unary>& unary) noexcept(false) {
  const auto operand = unary->operand();
  const token_t type = unary->type();
  const ast::type_t ast_type = operand->ast_type();
  if (auto result = eval_context::unary_implementation(ast_type, type, operand)) {
    return result;
  }
  do_typecheck(ast_type, ast::type_t::SYMBOL, "Unknown unary operand type");
  const auto variable = boost::static_pointer_cast<ast::Symbol>(operand);
  const auto& symbol = storage_.lookup(variable->name());
  storage_.overwrite(variable->name(), eval_context::unary_implementation(symbol->ast_type(), type, symbol));
  return symbol;
}

void Evaluator::eval_array(const boost::local_shared_ptr<ast::Array>& array) noexcept(false) {
  for (auto& element : array->elements()) {
    element = eval(element);
  }
}

void Evaluator::eval_for(const boost::local_shared_ptr<ast::For>& stmt) noexcept(false) {
  storage_.scope_begin();
  const auto init = eval(stmt->loop_init());
  const auto init_type = eval(boost::static_pointer_cast<ast::Binary>(init)->lhs())->ast_type();
  const auto& exit_cond = stmt->exit_condition();
  const auto& increment = stmt->increment();
  const auto& body = stmt->body();

  auto implementation = [this, &exit_cond, &increment, &body]<typename Numeric>() {
    auto exit_cond_ = boost::static_pointer_cast<Numeric>(eval(exit_cond));
    while (exit_cond_->value()) {
      eval(body);
      eval(increment);
      exit_cond_ = boost::static_pointer_cast<Numeric>(eval(exit_cond));
    }
  };
  if (init_type == ast::type_t::INTEGER) {
    implementation.template operator()<ast::Integer>();
  } else if (init_type == ast::type_t::FLOAT) {
    implementation.template operator()<ast::Float>();
  }
  storage_.scope_end();
}

void Evaluator::eval_while(const boost::local_shared_ptr<ast::While>& stmt) noexcept(false) {
  const auto exit_cond = eval(stmt->exit_condition());
  const auto exit_cond_type = exit_cond->ast_type();

  auto implementation = [this, &stmt, &exit_cond]<typename Numeric>() {
    const auto& body = stmt->body();
    auto exit_cond_ = boost::static_pointer_cast<Numeric>(exit_cond);
    while (exit_cond_->value()) {
      eval(body);
      exit_cond_ = boost::static_pointer_cast<Numeric>(eval(stmt->exit_condition()));
    }
  };
  if (exit_cond_type == ast::type_t::INTEGER) {
    implementation.template operator()<ast::Integer>();
  } else if (exit_cond_type == ast::type_t::FLOAT) {
    implementation.template operator()<ast::Float>();
  }
}

void Evaluator::eval_if(const boost::local_shared_ptr<ast::If>& stmt) noexcept(false) {
  const auto if_condition = eval(stmt->condition());
  if (boost::static_pointer_cast<ast::Integer>(if_condition)->value()) {
    eval(stmt->body());
  } else if (auto else_body = stmt->else_body()) {
    eval(else_body);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::eval(const boost::local_shared_ptr<ast::Object>& stmt) noexcept(false) {
  switch (stmt->ast_type()) {
    case ast::type_t::INTEGER:
    case ast::type_t::FLOAT:
    case ast::type_t::STRING:
    case ast::type_t::TYPE_OBJECT: {
      return stmt;
    }
    case ast::type_t::LAMBDA: {
      add_function(stmt, storage_);
      return stmt;
    }
    case ast::type_t::SYMBOL: {
      return storage_.lookup(boost::static_pointer_cast<ast::Symbol>(stmt)->name());
    }
    case ast::type_t::TYPE_FIELD: {
      return eval_type_field_access(boost::static_pointer_cast<ast::TypeFieldOperator>(stmt));
    }
    case ast::type_t::TYPE_CREATOR: {
      return eval_type_creation_function(boost::static_pointer_cast<ast::TypeCreator>(stmt));
    }
    case ast::type_t::LAMBDA_CALL: {
      return eval_function_call(boost::static_pointer_cast<ast::LambdaCall>(stmt));
    }
    case ast::type_t::BINARY: {
      return eval_binary(boost::static_pointer_cast<ast::Binary>(stmt));
    }
    case ast::type_t::UNARY: {
      return eval_unary(boost::static_pointer_cast<ast::Unary>(stmt));
    }
    case ast::type_t::ARRAY: {
      eval_array(boost::static_pointer_cast<ast::Array>(stmt));
      return stmt;
    }
    case ast::type_t::BLOCK: {
      eval_block(boost::static_pointer_cast<ast::Block>(stmt));
      return stmt;
    }
    case ast::type_t::WHILE: {
      eval_while(boost::static_pointer_cast<ast::While>(stmt));
      return stmt;
    }
    case ast::type_t::FOR: {
      eval_for(boost::static_pointer_cast<ast::For>(stmt));
      return stmt;
    }
    case ast::type_t::IF: {
      eval_if(boost::static_pointer_cast<ast::If>(stmt));
      return stmt;
    }
    default:
      throw EvalError("Unknown expression");
  }
}
