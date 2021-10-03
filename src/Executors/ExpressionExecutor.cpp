#include "ExpressionExecutor.h"

const std::set<ICodeNodeTypeImpl> ExpressionExecutor::mArithOps = {
    ICodeNodeTypeImpl::ADD, ICodeNodeTypeImpl::SUBTRACT,
    ICodeNodeTypeImpl::MULTIPLY, ICodeNodeTypeImpl::FLOAT_DIVIDE,
    ICodeNodeTypeImpl::INTEGER_DIVIDE};

ExpressionExecutor::ExpressionExecutor(Executor &executor)
    : SubExecutorBase(executor), mValueType(VariableType::UNKNOWN) {}

std::shared_ptr<SubExecutorBase> ExpressionExecutor::execute(
    const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
        &node) {
  const auto node_type = node->type();
  switch (node_type) {
  case ICodeNodeTypeImpl::VARIABLE: {
    const auto entry = std::any_cast<
        std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>(
        node->getAttribute(ICodeKeyTypeImpl::ID));
    mValue = entry->getAttribute(SymbolTableKeyTypeImpl::DATA_VALUE);
    mValueType = std::any_cast<VariableType>(
        entry->getAttribute(SymbolTableKeyTypeImpl::DATA_TYPE));
    break;
  }
  case ICodeNodeTypeImpl::INTEGER_CONSTANT: {
    mValue = node->getAttribute(ICodeKeyTypeImpl::VALUE);
    mValueType = VariableType::INTEGER;
    break;
  }
  case ICodeNodeTypeImpl::REAL_CONSTANT: {
    mValue = node->getAttribute(ICodeKeyTypeImpl::VALUE);
    mValueType = VariableType::FLOAT;
    break;
  }
  case ICodeNodeTypeImpl::STRING_CONSTANT: {
    mValue = node->getAttribute(ICodeKeyTypeImpl::VALUE);
    mValueType = VariableType::STRING;
    break;
  }
  case ICodeNodeTypeImpl::NEGATE: {
    auto children_it = node->childrenBegin();
    auto operand = *children_it;
    ExpressionExecutor executor(*currentExecutor());
    executor.execute(operand);
    mValue = executor.value();
    switch (executor.valueType()) {
    case VariableType::FLOAT: {
      mValue = -std::any_cast<double>(mValue);
      mValueType = VariableType::FLOAT;
      break;
    }
    case VariableType::INTEGER: {
      mValue = -std::any_cast<unsigned long long>(mValue);
      mValueType = VariableType::INTEGER;
      break;
    }
    default: {
      errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                           currentExecutor());
      break;
    }
    }
    break;
  }
  case ICodeNodeTypeImpl::NOT: {
    auto children_it = node->childrenBegin();
    auto operand = *children_it;
    ExpressionExecutor executor(*currentExecutor());
    executor.execute(operand);
    if (executor.valueType() == VariableType::BOOLEAN) {
      mValue = !std::any_cast<bool>(executor.value());
      mValueType = VariableType::BOOLEAN;
    } else {
      // implicit type conversion?
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

std::any ExpressionExecutor::value() const { return mValue; }

VariableType ExpressionExecutor::valueType() const { return mValueType; }

std::shared_ptr<SubExecutorBase> ExpressionExecutor::executeBinaryOperator(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> &node,
    const ICodeNodeTypeImpl node_type) {
  // execute the lhs and rhs expressions
  auto children_it = node->childrenBegin();
  auto operand_lhs = *children_it;
  ++children_it;
  auto operand_rhs = *children_it;
  ExpressionExecutor executor_lhs(*currentExecutor()),
      executor_rhs(*currentExecutor());
  executor_lhs.execute(operand_lhs);
  executor_rhs.execute(operand_rhs);
  const auto lhs_value = executor_lhs.value();
  const auto rhs_value = executor_rhs.value();
  // check if this is integer operator
  const auto type_lhs = executor_lhs.valueType();
  const auto type_rhs = executor_rhs.valueType();
  const bool integer_mode = (type_lhs == VariableType::INTEGER) &&
                            (type_rhs == VariableType::INTEGER);
  // check if the operator is in mArithOps
  const auto search = mArithOps.find(node_type);
  if (search != mArithOps.end()) {
    // arithmetic operators
    if (integer_mode) {
      const auto lhs_value_int = std::any_cast<unsigned long long>(lhs_value);
      const auto rhs_value_int = std::any_cast<unsigned long long>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::ADD: {
        mValue = lhs_value_int + rhs_value_int;
        mValueType = VariableType::INTEGER;
        break;
      }
      case ICodeNodeTypeImpl::SUBTRACT: {
        mValue = lhs_value_int - rhs_value_int;
        mValueType = VariableType::INTEGER;
        break;
      }
      case ICodeNodeTypeImpl::MULTIPLY: {
        mValue = lhs_value_int * rhs_value_int;
        mValueType = VariableType::INTEGER;
        break;
      }
      case ICodeNodeTypeImpl::FLOAT_DIVIDE: {
        if (rhs_value_int != 0) {
          mValue = static_cast<double>(lhs_value_int) /
                   static_cast<double>(rhs_value_int);
          mValueType = VariableType::FLOAT;
        } else {
          mValue = 0.0;
          mValueType = VariableType::FLOAT;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      case ICodeNodeTypeImpl::INTEGER_DIVIDE: {
        if (rhs_value_int != 0) {
          mValue = lhs_value_int / rhs_value_int;
          mValueType = VariableType::INTEGER;
        } else {
          mValue = 0;
          mValueType = VariableType::INTEGER;
          errorHandler()->flag(node, RuntimeErrorCode::DIVISION_BY_ZERO,
                               currentExecutor());
        }
        break;
      }
      case ICodeNodeTypeImpl::MOD: {
        if (rhs_value_int != 0) {
          mValue = lhs_value_int % rhs_value_int;
          mValueType = VariableType::INTEGER;
        } else {
          mValue = 0;
          mValueType = VariableType::INTEGER;
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
      const double lhs_value_float =
          (type_lhs == VariableType::INTEGER)
              ? static_cast<double>(
                    std::any_cast<unsigned long long>(lhs_value))
              : std::any_cast<double>(lhs_value);
      const double rhs_value_float =
          (type_rhs == VariableType::INTEGER)
              ? static_cast<double>(
                    std::any_cast<unsigned long long>(rhs_value))
              : std::any_cast<double>(rhs_value);
      mValueType = VariableType::FLOAT;
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
          mValue = static_cast<double>(lhs_value_float) /
                   static_cast<double>(rhs_value_float);
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
    const bool lhs_value_bool = std::any_cast<bool>(lhs_value);
    const bool rhs_value_bool = std::any_cast<bool>(rhs_value);
    mValueType = VariableType::BOOLEAN;
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
//      errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
//                           currentExecutor());
//      break;
    }
    }
  } else {
    // relational operators
    mValueType = VariableType::BOOLEAN;
    if (integer_mode) {
      const auto lhs_value_int = std::any_cast<unsigned long long>(lhs_value);
      const auto rhs_value_int = std::any_cast<unsigned long long>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::EQ: {
        mValue = (lhs_value_int == rhs_value_int);
        break;
      }
      case ICodeNodeTypeImpl::NE: {
        mValue = (lhs_value_int != rhs_value_int);
        break;
      }
      case ICodeNodeTypeImpl::LT: {
        mValue = (lhs_value_int < rhs_value_int);
        break;
      }
      case ICodeNodeTypeImpl::LE: {
        mValue = (lhs_value_int <= rhs_value_int);
        break;
      }
      case ICodeNodeTypeImpl::GT: {
        mValue = (lhs_value_int > rhs_value_int);
        break;
      }
      case ICodeNodeTypeImpl::GE: {
        mValue = (lhs_value_int >= rhs_value_int);
        break;
      }
      default: {
        errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE,
                             currentExecutor());
        break;
      }
      }
    } else {
      const double lhs_value_float =
          (type_lhs == VariableType::INTEGER)
              ? static_cast<double>(
                    std::any_cast<unsigned long long>(lhs_value))
              : std::any_cast<double>(lhs_value);
      const double rhs_value_float =
          (type_rhs == VariableType::INTEGER)
              ? static_cast<double>(
                    std::any_cast<unsigned long long>(rhs_value))
              : std::any_cast<double>(rhs_value);
      switch (node_type) {
      case ICodeNodeTypeImpl::EQ: {
        mValue = (lhs_value_float == rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::NE: {
        mValue = (lhs_value_float != rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::LT: {
        mValue = (lhs_value_float < rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::LE: {
        mValue = (lhs_value_float <= rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::GT: {
        mValue = (lhs_value_float > rhs_value_float);
        break;
      }
      case ICodeNodeTypeImpl::GE: {
        mValue = (lhs_value_float >= rhs_value_float);
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
