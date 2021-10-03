#ifndef STATEMENTEXECUTOR_H
#define STATEMENTEXECUTOR_H

#include "Interpreter.h"

class StatementExecutor : public SubExecutorBase
{
public:
  explicit StatementExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // STATEMENTEXECUTOR_H
