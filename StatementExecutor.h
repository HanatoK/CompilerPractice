#ifndef STATEMENTEXECUTOR_H
#define STATEMENTEXECUTOR_H

#include "Interpreter.h"

class StatementExecutor : public SubExecutorBase
{
public:
  StatementExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node);
};

#endif // STATEMENTEXECUTOR_H
