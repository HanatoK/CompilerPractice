#ifndef COMPOUNDEXECUTOR_H
#define COMPOUNDEXECUTOR_H

#include "Interpreter.h"

class CompoundStatementExecutor : public SubExecutorBase
{
public:
  explicit CompoundStatementExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node);
};

#endif // COMPOUNDEXECUTOR_H
