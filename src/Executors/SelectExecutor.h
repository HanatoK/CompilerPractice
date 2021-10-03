#ifndef SELECTEXECUTOR_H
#define SELECTEXECUTOR_H

#include "Interpreter.h"

class SelectExecutor : public SubExecutorBase
{
public:
  explicit SelectExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // SELECTEXECUTOR_H
