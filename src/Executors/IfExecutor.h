#ifndef IFEXECUTOR_H
#define IFEXECUTOR_H

#include "Interpreter.h"

class IfExecutor : public SubExecutorBase
{
public:
  explicit IfExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // IFEXECUTOR_H
