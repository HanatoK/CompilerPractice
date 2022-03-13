#ifndef ASSIGNMENTSTATEMENTEXECUTOR_H
#define ASSIGNMENTSTATEMENTEXECUTOR_H

#include "Interpreter.h"

class AssignmentStatementExecutor : public SubExecutorBase
{
public:
  explicit AssignmentStatementExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // ASSIGNMENTSTATEMENTEXECUTOR_H
