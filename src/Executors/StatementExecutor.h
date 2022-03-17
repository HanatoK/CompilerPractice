#ifndef STATEMENTEXECUTOR_H
#define STATEMENTEXECUTOR_H

#include "Interpreter.h"

class StatementExecutor : public SubExecutorBase
{
public:
  explicit StatementExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // STATEMENTEXECUTOR_H
