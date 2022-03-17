#ifndef IFEXECUTOR_H
#define IFEXECUTOR_H

#include "Interpreter.h"

class IfExecutor : public SubExecutorBase
{
public:
  explicit IfExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // IFEXECUTOR_H
