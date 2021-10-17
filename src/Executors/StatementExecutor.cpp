#include "StatementExecutor.h"
#include "CompoundStatementExecutor.h"
#include "AssignmentStatementExecutor.h"
#include "LoopExecutor.h"
#include "IfExecutor.h"
#include "SelectExecutor.h"

StatementExecutor::StatementExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> StatementExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  auto node_type = node->type();
  sendSourceLineMessage(node);
  switch (node_type) {
  case ICodeNodeTypeImpl::COMPOUND: {
    CompoundStatementExecutor compound_executor(*currentExecutor());
    return compound_executor.execute(node);
  }
  case ICodeNodeTypeImpl::ASSIGN: {
    AssignmentStatementExecutor assignment_executor(*currentExecutor());
    return assignment_executor.execute(node);
  }
  case ICodeNodeTypeImpl::LOOP: {
    LoopExecutor loop_executor(*currentExecutor());
    return loop_executor.execute(node);
  }
  case ICodeNodeTypeImpl::IF: {
    IfExecutor if_executor(*currentExecutor());
    return if_executor.execute(node);
  }
  case ICodeNodeTypeImpl::SELECT: {
//    SelectExecutor select_executor(*currentExecutor());
    SelectExecutorOpt select_executor(*currentExecutor());
    return select_executor.execute(node);
  }
  case ICodeNodeTypeImpl::NO_OP: {
    return nullptr;
  }
  default: {
    errorHandler()->flag(node, RuntimeErrorCode::UNIMPLEMENTED_FEATURE, currentExecutor());
  }
  }
  return nullptr;
}
