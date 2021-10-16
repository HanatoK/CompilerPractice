#include "IfExecutor.h"
#include "ExpressionExecutor.h"
#include "StatementExecutor.h"

IfExecutor::IfExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> IfExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  auto it_child = node->childrenBegin();
  auto expr_node = *it_child++;
  auto then_statement_node = *it_child++;
  auto else_statement_node = (it_child == node->childrenEnd() ? nullptr : *it_child);
  ExpressionExecutor expression_executor(*currentExecutor());
  expression_executor.execute(expr_node);
  auto b = std::any_cast<bool>(expression_executor.value());
  StatementExecutor statement_executor(*currentExecutor());
  if (b) {
    statement_executor.execute(then_statement_node);
  } else {
    if (else_statement_node != nullptr) {
      statement_executor.execute(else_statement_node);
    }
  }
  ++executionCount();
  return nullptr;
}
