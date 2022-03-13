#include "AssignmentStatementExecutor.h"
#include "ExpressionExecutor.h"

AssignmentStatementExecutor::AssignmentStatementExecutor(const std::shared_ptr<Executor>& executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> AssignmentStatementExecutor::execute(const std::shared_ptr<ICodeNodeImplBase>& node)
{
  // get the variable node and the expression node
  auto it = node->childrenBegin();
  auto variable_node = *it;
  ++it;
  auto expression_node = *it;
  // execute the expression node
  ExpressionExecutor expression_executor(currentExecutor());
  expression_executor.execute(expression_node);
  auto expression_value = expression_executor.value();
  const auto variable_id = cast_by_enum<ICodeKeyTypeImpl::ID>(variable_node->getAttribute(ICodeKeyTypeImpl::ID));
  switch (expression_executor.valueType()) {
    case VariableInternalType::INTEGER: {
      variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_VALUE, std::get<PascalInteger>(expression_value));
      break;
    }
    case VariableInternalType::BOOLEAN: {
      variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_VALUE, std::get<bool>(expression_value));
      break;
    }
    case VariableInternalType::REAL: {
      variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_VALUE, std::get<PascalFloat>(expression_value));
      break;
    }
    case VariableInternalType::STRING: {
      variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_VALUE, std::get<std::string>(expression_value));
      break;
    }
    default: {
      errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE, currentExecutor());
      break;
    }
  }
  variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_INTERNAL_TYPE, expression_executor.valueType());
  const auto line_number_attribute = node->getAttribute(ICodeKeyTypeImpl::LINE);
  if (line_number_attribute.has_value()) {
    const auto line_number = cast_by_enum<ICodeKeyTypeImpl::LINE>(line_number_attribute);
    currentExecutor()->assignmentMessage(line_number, variable_id->name(), expression_value);
  }
  ++executionCount();
  return nullptr;
}
