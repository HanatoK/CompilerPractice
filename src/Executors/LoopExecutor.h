#ifndef LOOPEXECUTOR_H
#define LOOPEXECUTOR_H

#include "Interpreter.h"

class LoopExecutor : public SubExecutorBase
{
public:
  explicit LoopExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // LOOPEXECUTOR_H
