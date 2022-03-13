#ifndef COMPOUNDEXECUTOR_H
#define COMPOUNDEXECUTOR_H

#include "Interpreter.h"

class CompoundStatementExecutor : public SubExecutorBase
{
public:
  explicit CompoundStatementExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // COMPOUNDEXECUTOR_H
