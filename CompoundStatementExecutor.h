#ifndef COMPOUNDEXECUTOR_H
#define COMPOUNDEXECUTOR_H

#include "Interpreter.h"

class CompoundStatementExecutor : public SubExecutorBase
{
public:
  CompoundStatementExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node);
};

#endif // COMPOUNDEXECUTOR_H
