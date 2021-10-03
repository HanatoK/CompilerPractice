#include "SelectExecutor.h"

SelectExecutor::SelectExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> SelectExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  return nullptr;
}
