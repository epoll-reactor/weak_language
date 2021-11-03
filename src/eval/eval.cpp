#include "../../include/eval/eval.hpp"

#include "../../include/cut_last_iterator.hpp"
#include "../../include/eval/implementation/binary.hpp"
#include "../../include/eval/implementation/unary.hpp"
#include "../../include/std/builtins.hpp"

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
/// For some reason this lambda works incorrect with ALWAYS_INLINE specifier
static bool add_lambda(const boost::local_shared_ptr<ast::Object>& object, Storage& storage) noexcept(false) {
  if (const auto lambda = boost::dynamic_pointer_cast<ast::Lambda>(object)) {
    storage.push(lambda->name().data(), lambda);
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
    if (add_lambda(expr, storage_)) {
      continue;
    }
    add_type_definition(boost::dynamic_pointer_cast<ast::TypeDefinition>(expr));
  }
  call_lambda("main", {});
}

void Evaluator::add_type_definition(const boost::local_shared_ptr<ast::TypeDefinition>& definition) noexcept(false) {
  type_creators_.emplace(definition->name(), [&definition](const std::vector<boost::local_shared_ptr<ast::Object>>& names) {
    const auto& type_names = definition->fields();
    if (type_names.size() != names.size()) {
      throw EvalError("new {}: wrong arguments size", definition->name());
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

boost::local_shared_ptr<ast::Object> Evaluator::eval_type_creation(const boost::local_shared_ptr<ast::TypeCreator>& type_creator) noexcept(false) {
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

boost::local_shared_ptr<ast::Object> Evaluator::call_lambda(std::string_view name, const std::vector<boost::local_shared_ptr<ast::Object>>& arguments) noexcept(false) {
  auto find_lambda = [this, &name] {
    const auto& lambda = storage_.lookup(name.data()).get();
    do_typecheck(lambda, ast::type_t::LAMBDA, "Try to call not a lambda");
    return static_cast<ast::Lambda*>(lambda);
  };

  const auto lambda = find_lambda();
  const auto& call_args_names = lambda->arguments();
  const auto& body = lambda->body()->statements();
  if (body.empty()) {
    return {};
  }
  if (call_args_names.size() != arguments.size()) {
    throw EvalError("Wrong arguments size");
  }
  storage_.scope_begin();
  for (size_t i = 0; i < arguments.size(); ++i) {
    const std::string& argument_name = static_cast<ast::Symbol*>(call_args_names[i].get())->name();
    storage_.push(argument_name, arguments[i]);
  }
  for (const auto& statement : cut_last(body)) {
    eval(statement);
  }
  auto last_statement = eval(*--body.cend());
  if (is_datatype(last_statement.get())) {
    storage_.scope_end();
    return last_statement;
  }
  storage_.scope_end();
  return {};
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_lambda_call(const boost::local_shared_ptr<ast::LambdaCall>& lambda_call) noexcept(false) {
  auto arguments = lambda_call->arguments();
  for (auto& argument : arguments) {
    const auto type = argument->ast_type();
    if (type != ast::type_t::INTEGER && type != ast::type_t::FLOAT && type != ast::type_t::STRING) {
      argument = eval(argument);
    }
  }
  const std::string& name = lambda_call->name();
  if (builtins.contains(name)) {
    if (const auto result = builtins.at(name)(arguments)) {
      return result.value();
    } else {
      return nullptr;
    }
  }
  return call_lambda(name, arguments);
}

void Evaluator::eval_block(const boost::local_shared_ptr<ast::Block>& block) noexcept(false) {
  for (const auto& statement : block->statements()) {
    eval(statement);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_binary(const boost::local_shared_ptr<ast::Binary>& binary) noexcept(false) {
  const token_t type = binary->type();
  if (type == token_t::ASSIGN) {
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
  return eval_context::binary_implementation(lhs->ast_type(), rhs->ast_type(), type, lhs, rhs);
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_unary(const boost::local_shared_ptr<ast::Unary>& unary) noexcept(false) {
  auto& operand = unary->operand();
  const token_t type = unary->type();
  const ast::type_t ast_type = operand->ast_type();
  if (bool failed = false; eval_context::unary_implementation(type, operand, failed), !failed) {
    return operand;
  }
  do_typecheck(ast_type, ast::type_t::SYMBOL, "Unknown unary operand type");
  const auto variable = boost::static_pointer_cast<ast::Symbol>(operand);
  auto& symbol = storage_.lookup(variable->name());
  if (bool failed = false; eval_context::unary_implementation(type, symbol, failed), !failed) {
    storage_.overwrite(variable->name(), symbol);
  }
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
  const auto& body = stmt->body();
  auto implementation = [this, &stmt, &exit_cond, &body]<typename Numeric>() {
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
  using ast::type_t;
  switch (stmt->ast_type()) {
    case type_t::INTEGER:
    case type_t::FLOAT:
    case type_t::STRING:
    case type_t::TYPE_OBJECT: {
      return stmt;
    }
    case type_t::LAMBDA: {
      add_lambda(stmt, storage_);
      return stmt;
    }
    case type_t::SYMBOL: {
      return storage_.lookup(boost::static_pointer_cast<ast::Symbol>(stmt)->name());
    }
    case type_t::TYPE_FIELD: {
      return eval_type_field_access(boost::static_pointer_cast<ast::TypeFieldOperator>(stmt));
    }
    case type_t::TYPE_CREATOR: {
      return eval_type_creation(boost::static_pointer_cast<ast::TypeCreator>(stmt));
    }
    case type_t::LAMBDA_CALL: {
      return eval_lambda_call(boost::static_pointer_cast<ast::LambdaCall>(stmt));
    }
    case type_t::BINARY: {
      return eval_binary(boost::static_pointer_cast<ast::Binary>(stmt));
    }
    case type_t::UNARY: {
      return eval_unary(boost::static_pointer_cast<ast::Unary>(stmt));
    }
    case type_t::ARRAY: {
      eval_array(boost::static_pointer_cast<ast::Array>(stmt));
      return stmt;
    }
    case type_t::BLOCK: {
      eval_block(boost::static_pointer_cast<ast::Block>(stmt));
      return stmt;
    }
    case type_t::WHILE: {
      eval_while(boost::static_pointer_cast<ast::While>(stmt));
      return stmt;
    }
    case type_t::FOR: {
      eval_for(boost::static_pointer_cast<ast::For>(stmt));
      return stmt;
    }
    case type_t::IF: {
      eval_if(boost::static_pointer_cast<ast::If>(stmt));
      return stmt;
    }
    default:
      throw EvalError("Unknown expression");
  }
}
