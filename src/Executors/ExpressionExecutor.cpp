#include "ExpressionExecutor.h"

const std::set<ICodeNodeTypeImpl> ExpressionExecutor::mArithOps = {
    ICodeNodeTypeImpl::ADD, ICodeNodeTypeImpl::SUBTRACT,
    ICodeNodeTypeImpl::MULTIPLY, ICodeNodeTypeImpl::FLOAT_DIVIDE,
    ICodeNodeTypeImpl::INTEGER_DIVIDE};

ExpressionExecutor::ExpressionExecutor(const std::shared_ptr<Executor>& executor)
    : SubExecutorBase(executor) {}

std::shared_ptr<SubExecutorBase> ExpressionExecutor::execute(
    const std::shared_ptr<ICodeNodeImplBase> &node) {
  const auto node_type = node->type();
  switch (node_type) {
  case ICodeNodeTypeImpl::VARIABLE: {
    const auto entry = cast_by_enum<ICodeKeyTypeImpl::ID>(node->getAttribute(ICodeKeyTypeImpl::ID));
    mValue = cast_by_enum<SymbolTableKeyTypeImpl::DATA_VALUE>(entry->getAttribute(SymbolTableKeyTypeImpl::DATA_VALUE));
    break;
  }
  case ICodeNodeTypeImpl::INTEGER_CONSTANT: {
    mValue = std::any_cast<PascalInteger>(node->getAttribute(ICodeKeyTypeImpl::VALUE));
    break;
  }
  case ICodeNodeTypeImpl::REAL_CONSTANT: {
    mValue = std::any_cast<PascalFloat>(node->getAttribute(ICodeKeyTypeImpl::VALUE));
    break;
  }
  case ICodeNodeTypeImpl::STRING_CONSTANT: {
    mValue = std::any_cast<std::string>(node->getAttribute(ICodeKeyTypeImpl::VALUE));
    break;
  }
  case ICodeNodeTypeImpl::NEGATE: {
    auto children_it = node->childrenBegin();
    auto operand = *children_it;
    ExpressionExecutor executor(currentExecutor());
    executor.execute(operand);
    mValue = executor.value();
    std::visit(overloaded{
       [](PascalFloat& x){x = -x;},
       [](PascalInteger& x){x = -x;},
       [&](auto& x){
         errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE, currentExecutor());
       },
     }, mValue);
    break;
  }
  case ICodeNodeTypeImpl::NOT: {
    auto children_it = node->childrenBegin();
    auto operand = *children_it;
    ExpressionExecutor executor(currentExecutor());
    executor.execute(operand);
    if (std::holds_alternative<bool>(executor.value())) {
      mValue = !std::get<bool>(executor.value());
    } else {
      errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                           currentExecutor());
    }
    break;
  }
  default: {
    return executeBinaryOperator(node, node_type);
  }
  }
  return nullptr;
}

VariableValueT ExpressionExecutor::value() const { return mValue; }


std::shared_ptr<SubExecutorBase> ExpressionExecutor::executeBinaryOperator(const std::shared_ptr<ICodeNodeImplBase>& node,
    const ICodeNodeTypeImpl node_type) {
  // execute the lhs and rhs expressions
  auto children_it = node->childrenBegin();
  auto operand_lhs = *children_it;
  ++children_it;
  auto operand_rhs = *children_it;
  ExpressionExecutor executor_lhs(currentExecutor()),
      executor_rhs(currentExecutor());
  executor_lhs.execute(operand_lhs);
  executor_rhs.execute(operand_rhs);
  const auto lhs_value = executor_lhs.value();
  const auto rhs_value = executor_rhs.value();
  // check if this is integer operator
  const bool integer_mode = std::holds_alternative<PascalInteger>(lhs_value) &&
                            std::holds_alternative<PascalInteger>(rhs_value);
  // check if the operator is in mArithOps
  if (mArithOps.contains(node_type)) {
    // arithmetic operators
    if (integer_mode) {
      const auto lhs_value_int = std::get<PascalInteger>(lhs_value);
      const auto rhs_value_int = std::get<PascalInteger>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::ADD: {
        mValue = lhs_value_int + rhs_value_int;
        break;
      }
      case ICodeNodeTypeImpl::SUBTRACT: {
        mValue = lhs_value_int - rhs_value_int;
        break;
      }
      case ICodeNodeTypeImpl::MULTIPLY: {
        mValue = lhs_value_int * rhs_value_int;
        break;
      }
      case ICodeNodeTypeImpl::FLOAT_DIVIDE: {
        if (rhs_value_int != 0) {
          mValue = static_cast<PascalFloat>(lhs_value_int) /
                   static_cast<PascalFloat>(rhs_value_int);
        } else {
          mValue = 0.0;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      case ICodeNodeTypeImpl::INTEGER_DIVIDE: {
        if (rhs_value_int != 0) {
          mValue = lhs_value_int / rhs_value_int;
        } else {
          mValue = 0;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      case ICodeNodeTypeImpl::MOD: {
        if (rhs_value_int != 0) {
          mValue = lhs_value_int % rhs_value_int;
        } else {
          mValue = 0;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      default: {
        errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                             currentExecutor());
        break;
      }
      }
    } else {
      const PascalFloat lhs_value_float =
          std::holds_alternative<PascalInteger>(lhs_value)
              ? static_cast<PascalFloat>(
                    std::get<PascalInteger>(lhs_value))
              : std::get<PascalFloat>(lhs_value);
      const PascalFloat rhs_value_float =
          std::holds_alternative<PascalInteger>(rhs_value)
              ? static_cast<PascalFloat>(
                    std::get<PascalInteger>(rhs_value))
              : std::get<PascalFloat>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::ADD: {
        mValue = lhs_value_float + rhs_value_float;
        break;
      }
      case ICodeNodeTypeImpl::SUBTRACT: {
        mValue = lhs_value_float - rhs_value_float;
        break;
      }
      case ICodeNodeTypeImpl::MULTIPLY: {
        mValue = lhs_value_float * rhs_value_float;
        break;
      }
      case ICodeNodeTypeImpl::FLOAT_DIVIDE: {
        // TODO: use std::numeric_limits
        if (rhs_value_float != 0.0) {
          mValue = static_cast<PascalFloat>(lhs_value_float) /
                   static_cast<PascalFloat>(rhs_value_float);
        } else {
          mValue = 0.0;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      default: {
        errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                             currentExecutor());
        break;
      }
      }
    }
  } else if (node_type == ICodeNodeTypeImpl::AND ||
             node_type == ICodeNodeTypeImpl::OR) {
    // and and or
    const bool lhs_value_bool = std::get<bool>(lhs_value);
    const bool rhs_value_bool = std::get<bool>(rhs_value);
    switch (node_type) {
      case ICodeNodeTypeImpl::AND: {
        mValue = lhs_value_bool && rhs_value_bool;
        break;
      }
      case ICodeNodeTypeImpl::OR: {
        mValue = lhs_value_bool || rhs_value_bool;
        break;
      }
      default: {
        errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                             currentExecutor());
        break;
      }
    }
  } else {
    // relational operators
    if (integer_mode) {
      const auto lhs_value_int = std::get<PascalInteger>(lhs_value);
      const auto rhs_value_int = std::get<PascalInteger>(rhs_value);
      switch (node_type) {
        case ICodeNodeTypeImpl::EQ: {
          mValue = bool(lhs_value_int == rhs_value_int);
          break;
        }
        case ICodeNodeTypeImpl::NE: {
          mValue = bool(lhs_value_int != rhs_value_int);
          break;
        }
        case ICodeNodeTypeImpl::LT: {
          mValue = bool(lhs_value_int < rhs_value_int);
          break;
        }
        case ICodeNodeTypeImpl::LE: {
          mValue = bool(lhs_value_int <= rhs_value_int);
          break;
        }
        case ICodeNodeTypeImpl::GT: {
          mValue = bool(lhs_value_int > rhs_value_int);
          break;
        }
        case ICodeNodeTypeImpl::GE: {
          mValue = bool(lhs_value_int >= rhs_value_int);
          break;
        }
        default: {
          errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                               currentExecutor());
          break;
        }
      }
    } else {
      const PascalFloat lhs_value_float =
          std::holds_alternative<PascalInteger>(lhs_value)
              ? static_cast<PascalFloat>(
                    std::get<PascalInteger>(lhs_value))
              : std::get<PascalFloat>(lhs_value);
      const PascalFloat rhs_value_float =
          std::holds_alternative<PascalInteger>(rhs_value)
              ? static_cast<PascalFloat>(
                    std::get<PascalInteger>(rhs_value))
              : std::get<PascalFloat>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::EQ: {
        mValue = bool(lhs_value_float == rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::NE: {
        mValue = bool(lhs_value_float != rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::LT: {
        mValue = bool(lhs_value_float < rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::LE: {
        mValue = bool(lhs_value_float <= rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::GT: {
        mValue = bool(lhs_value_float > rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::GE: {
        mValue = bool(lhs_value_float >= rhs_value_float);
        break;
      }
      default: {
        errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                             currentExecutor());
        break;
      }
      }
    }
  }
  // should never get there
  return nullptr;
}
