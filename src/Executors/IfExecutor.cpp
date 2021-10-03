#include "IfExecutor.h"

IfExecutor::IfExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> IfExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  return nullptr;
}
