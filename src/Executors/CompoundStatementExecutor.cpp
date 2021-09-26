#include "CompoundStatementExecutor.h"
#include "StatementExecutor.h"

CompoundStatementExecutor::CompoundStatementExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> CompoundStatementExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> >& node)
{
  StatementExecutor statement_executor(*currentExecutor());
  for (auto it = node->childrenBegin(); it != node->childrenEnd(); ++it) {
    statement_executor.execute(*it);
  }
  return nullptr;
}
