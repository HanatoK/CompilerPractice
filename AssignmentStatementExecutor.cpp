#include "AssignmentStatementExecutor.h"
#include "ExpressionExecutor.h"

AssignmentStatementExecutor::AssignmentStatementExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> AssignmentStatementExecutor::execute(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > node)
{
  // get the variable node and the expression node
  auto it = node->childrenBegin();
  auto variable_node = *it;
  ++it;
  auto expression_node = *it;
  // execute the expression node
  ExpressionExecutor expression_executor(*currentExecutor());
  expression_executor.execute(expression_node);
  auto expression_value = expression_executor.value();
  const auto variable_id = std::any_cast<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>(variable_node->getAttribute(ICodeKeyTypeImpl::ID));
  variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_VALUE, expression_value);
  variable_id->setAttribute(SymbolTableKeyTypeImpl::DATA_TYPE, expression_executor.valueType());
  const auto line_number_attribute = node->getAttribute(ICodeKeyTypeImpl::LINE);
  if (line_number_attribute.has_value()) {
    const int line_number = std::any_cast<int>(line_number_attribute);
    currentExecutor()->assignmentMessage(line_number, variable_id->name(), expression_value);
  }
  ++executionCount();
  return nullptr;
}
