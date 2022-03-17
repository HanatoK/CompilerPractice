#ifndef LOOPEXECUTOR_H
#define LOOPEXECUTOR_H

#include "Interpreter.h"

class LoopExecutor : public SubExecutorBase
{
public:
  explicit LoopExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // LOOPEXECUTOR_H
