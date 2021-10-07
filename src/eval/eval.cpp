#include "../../include/eval/eval.hpp"

#include "../../include/eval/implementation/binary.hpp"
#include "../../include/eval/implementation/unary.hpp"
#include "../../include/std/builtins.hpp"

#include <numeric>

ALWAYS_INLINE static constexpr bool is_datatype(const ast::Object* object) noexcept(true) {
  if (!object) {
    return false;
  }
  switch (object->ast_type()) {
    case ast::ast_type_t::INTEGER:
    case ast::ast_type_t::FLOAT:
    case ast::ast_type_t::STRING:
    case ast::ast_type_t::ARRAY:
    case ast::ast_type_t::TYPE_OBJECT:
      return true;
    default:
      return false;
  }
}

template <ast::ast_type_t ExpectedType>
ALWAYS_INLINE static void do_typecheck(const ast::Object* object, const char* error_message) noexcept(false) {
  if (object->ast_type() != ExpectedType) {
    throw EvalError(error_message);
  }
}

template <ast::ast_type_t ExpectedType>
ALWAYS_INLINE static void do_typecheck(ast::ast_type_t target_type, const char* error_message) noexcept(false) {
  if (ExpectedType != target_type) {
    throw EvalError(error_message);
  }
}

Evaluator::Evaluator(const boost::local_shared_ptr<ast::RootObject>& program) noexcept(false) {
  for (const auto& stmt : program->get()) {
    expressions_.emplace_back(stmt);
  }
}

void Evaluator::eval() noexcept(false) {
  for (const auto& expr : expressions_) {
    if (const auto function = boost::dynamic_pointer_cast<ast::Function>(expr)) {
      storage_.push(function->name().data(), function);
      continue;
    }
    const auto type_def = boost::dynamic_pointer_cast<ast::TypeDefinition>(expr);
    add_type_definition(type_def);
  }
  call_function("main", {});
}

void Evaluator::add_type_definition(const boost::local_shared_ptr<ast::TypeDefinition>& definition) noexcept(false) {
  type_creators_.emplace(definition->name(), [&definition](const std::vector<boost::local_shared_ptr<ast::Object>>& evaluated_fields) {
    const auto& type_list = definition->fields();
    if (type_list.size() != evaluated_fields.size()) {
      throw EvalError("new {}: wrong arguments size", definition->name());
    }
    std::vector<std::pair<std::string, boost::local_shared_ptr<ast::Object>>> fields;
    for (size_t i = 0; i < type_list.size(); ++i) {
      fields.emplace_back(type_list[i], evaluated_fields[i]);
    }
    return boost::make_local_shared<ast::TypeObject>(std::move(fields));
  });
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_type_creation_function(const boost::local_shared_ptr<ast::TypeCreator>& type_creator) noexcept(false) {
  return type_creators_[type_creator->name()](type_creator->arguments());
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_type_field(const boost::local_shared_ptr<ast::TypeFieldOperator>& type_field) noexcept(false) {
  const auto& name = type_field->name();
  const auto& field = type_field->field();
  const auto& object = storage_.lookup(name);

  if (object->ast_type() != ast::ast_type_t::TYPE_OBJECT) {
    throw EvalError("Type object expected");
  }

  const auto type_object = boost::static_pointer_cast<ast::TypeObject>(object);
  const auto& fields = type_object->fields();
  auto found = std::find_if(fields.begin(), fields.end(), [&field](auto&& element) {
    return element.first == field;
  });
  if (found != fields.end()) {
    return found->second;
  } else {
    throw EvalError("{}: field not found - {}", name, field);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::call_function(std::string_view name, std::vector<boost::local_shared_ptr<ast::Object>> evaluated_args) noexcept(false) {
  auto find_function = [this, &name] {
    const auto& stored_function = storage_.lookup(name.data()).get();
    do_typecheck<ast::ast_type_t::FUNCTION>(stored_function, "Try to call not a function");
    return static_cast<ast::Function*>(stored_function);
  };

  const auto function = find_function();
  const auto& args = function->arguments();
  const auto& body = function->body()->statements();

  if (body.empty()) {
    return {};
  }
  if (args.size() != evaluated_args.size()) {
    throw EvalError("Wrong arguments size");
  }
  storage_.scope_begin();
  for (size_t i = 0; i < args.size(); ++i) {
    const std::string argument_name = static_cast<ast::Symbol*>(args[i].get())->name();
    storage_.push(argument_name, evaluated_args[i]);
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

boost::local_shared_ptr<ast::Object> Evaluator::eval_function_call(const boost::local_shared_ptr<ast::FunctionCall>& function_call) noexcept(false) {
  auto arguments = function_call->arguments();
  for (auto& arg : arguments) {
    auto type = arg->ast_type();
    if (type != ast::ast_type_t::INTEGER && type != ast::ast_type_t::FLOAT && type != ast::ast_type_t::STRING) {
      arg = eval(arg);
    }
  }
  const std::string& function_name = function_call->name();
  if (builtins.contains(function_name)) {
    if (const auto function_result = builtins.at(function_name)(arguments)) {
      return function_result.value();
    } else {
      return nullptr;
    }
  }
  return call_function(function_name, arguments);
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
        internal::assign_binary_implementation(type, storage_.lookup(variable->name()), eval(binary->rhs())));
    return variable;
  }

  const auto lhs = eval(binary->lhs());
  const auto rhs = eval(binary->rhs());

  const ast::ast_type_t lhs_t = lhs->ast_type();
  const ast::ast_type_t rhs_t = rhs->ast_type();

  using ast_t = ast::ast_type_t;
  if (lhs_t == ast_t::INTEGER && rhs_t == ast_t::INTEGER) {
    return internal::binary_implementation<ast::Integer, ast::Integer>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::INTEGER && rhs_t == ast_t::FLOAT) {
    return internal::binary_implementation<ast::Integer, ast::Float>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::FLOAT && rhs_t == ast_t::INTEGER) {
    return internal::binary_implementation<ast::Float, ast::Integer>(type, lhs, rhs);
  }
  if (lhs_t == ast_t::FLOAT && rhs_t == ast_t::FLOAT) {
    return internal::binary_implementation<ast::Float, ast::Float>(type, lhs, rhs);
  }
  throw EvalError("Unknown binary expr");
}

boost::local_shared_ptr<ast::Object> Evaluator::eval_unary(const boost::local_shared_ptr<ast::Unary>& unary) noexcept(false) {
  const auto operand = unary->operand();
  const token_t type = unary->type();
  const ast::ast_type_t ast_type = operand->ast_type();
  if (auto result = internal::unary_implementation(ast_type, type, operand)) {
    return result;
  }
  do_typecheck<ast::ast_type_t::SYMBOL>(ast_type, "Unknown unary operand type");
  const auto variable = boost::static_pointer_cast<ast::Symbol>(operand);
  const auto& symbol = storage_.lookup(variable->name());
  storage_.overwrite(variable->name(), internal::unary_implementation(symbol->ast_type(), type, symbol));
  return symbol;
}

void Evaluator::eval_array(const boost::local_shared_ptr<ast::Array>& array) noexcept(false) {
  for (auto& element : array->elements()) {
    element = eval(element);
  }
}

const boost::local_shared_ptr<ast::Object>& Evaluator::eval_array_subscript(const boost::local_shared_ptr<ast::ArraySubscriptOperator>& argument) noexcept(false) {
  const auto& array_object = storage_.lookup(argument->symbol_name()).get();
  do_typecheck<ast::ast_type_t::ARRAY>(array_object, "Try to subscript non-array expression");

  const auto index_object = eval(argument->index()).get();
  do_typecheck<ast::ast_type_t::INTEGER>(index_object, "Array subscript is not an integer");

  const auto array = static_cast<ast::Array*>(array_object);
  const auto index = static_cast<ast::Integer*>(index_object);
  const auto& elements = array->elements();
  const auto numeric_index = index->value();

  if (numeric_index >= elements.size()) {
    throw EvalError("Out of range");
  }
  return elements[numeric_index];
}

void Evaluator::eval_for(const boost::local_shared_ptr<ast::For>& for_stmt) noexcept(false) {
  storage_.scope_begin();
  const auto init = eval(for_stmt->loop_init());
  const auto init_type = eval(boost::static_pointer_cast<ast::Binary>(init)->lhs())->ast_type();
  const auto& exit_cond = for_stmt->exit_condition();
  const auto& increment = for_stmt->increment();
  const auto& body = for_stmt->body();

  auto implementation = [this, &exit_cond, &increment, &body]<typename Numeric>() {
    auto boolean_exit_condition = boost::static_pointer_cast<Numeric>(eval(exit_cond));
    while (boolean_exit_condition->value()) {
      eval(body);
      eval(increment);
      boolean_exit_condition = boost::static_pointer_cast<Numeric>(eval(exit_cond));
    }
  };
  if (init_type == ast::ast_type_t::INTEGER) {
    implementation.template operator()<ast::Integer>();
  } else if (init_type == ast::ast_type_t::FLOAT) {
    implementation.template operator()<ast::Float>();
  }
  storage_.scope_end();
}

void Evaluator::eval_while(const boost::local_shared_ptr<ast::While>& while_stmt) noexcept(false) {
  const auto exit_cond = eval(while_stmt->exit_condition());
  const auto exit_cond_type = exit_cond->ast_type();

  auto implementation = [this, &while_stmt, &exit_cond]<typename Numeric>() {
    const auto& body = while_stmt->body();
    auto internal_exit_cond = boost::static_pointer_cast<Numeric>(exit_cond);
    while (internal_exit_cond->value()) {
      eval(body);
      internal_exit_cond = boost::static_pointer_cast<Numeric>(eval(while_stmt->exit_condition()));
    }
  };
  if (exit_cond_type == ast::ast_type_t::INTEGER) {
    implementation.template operator()<ast::Integer>();
  } else if (exit_cond_type == ast::ast_type_t::FLOAT) {
    implementation.template operator()<ast::Float>();
  }
}

void Evaluator::eval_if(const boost::local_shared_ptr<ast::If>& if_stmt) noexcept(false) {
  const auto if_condition = eval(if_stmt->condition());
  if (boost::static_pointer_cast<ast::Integer>(if_condition)->value()) {
    eval(if_stmt->body());
  }
  if (auto else_body = if_stmt->else_body()) {
    eval(else_body);
  }
}

boost::local_shared_ptr<ast::Object> Evaluator::eval(const boost::local_shared_ptr<ast::Object>& expression) noexcept(false) {
  const ast::ast_type_t expr_type = expression->ast_type();

  switch (expr_type) {
    case ast::ast_type_t::INTEGER:
    case ast::ast_type_t::FLOAT:
    case ast::ast_type_t::STRING:
    case ast::ast_type_t::TYPE_OBJECT:
      return expression;

    case ast::ast_type_t::SYMBOL:
      return storage_.lookup(boost::static_pointer_cast<ast::Symbol>(expression)->name());

    case ast::ast_type_t::TYPE_FIELD:
      return eval_type_field(boost::static_pointer_cast<ast::TypeFieldOperator>(expression));

    case ast::ast_type_t::TYPE_CREATOR:
      return eval_type_creation_function(boost::static_pointer_cast<ast::TypeCreator>(expression));

    case ast::ast_type_t::FUNCTION_CALL:
      return eval_function_call(boost::static_pointer_cast<ast::FunctionCall>(expression));

    case ast::ast_type_t::BINARY:
      return eval_binary(boost::static_pointer_cast<ast::Binary>(expression));

    case ast::ast_type_t::UNARY:
      return eval_unary(boost::static_pointer_cast<ast::Unary>(expression));

    case ast::ast_type_t::ARRAY_SUBSCRIPT_OPERATOR:
      return eval_array_subscript(boost::static_pointer_cast<ast::ArraySubscriptOperator>(expression));

    case ast::ast_type_t::ARRAY:
      eval_array(boost::static_pointer_cast<ast::Array>(expression));
      return expression;

    case ast::ast_type_t::BLOCK:
      eval_block(boost::static_pointer_cast<ast::Block>(expression));
      return expression;

    case ast::ast_type_t::WHILE:
      eval_while(boost::static_pointer_cast<ast::While>(expression));
      return expression;

    case ast::ast_type_t::FOR:
      eval_for(boost::static_pointer_cast<ast::For>(expression));
      return expression;

    case ast::ast_type_t::IF:
      eval_if(boost::static_pointer_cast<ast::If>(expression));
      return expression;

    default:
      throw EvalError("Unknown expression");
  }
}
