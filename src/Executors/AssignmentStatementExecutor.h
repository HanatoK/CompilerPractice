#ifndef ASSIGNMENTSTATEMENTEXECUTOR_H
#define ASSIGNMENTSTATEMENTEXECUTOR_H

#include "Interpreter.h"

class AssignmentStatementExecutor : public SubExecutorBase
{
public:
  AssignmentStatementExecutor(Executor& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // ASSIGNMENTSTATEMENTEXECUTOR_H
