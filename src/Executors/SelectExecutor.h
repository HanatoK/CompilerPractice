#ifndef SELECTEXECUTOR_H
#define SELECTEXECUTOR_H

#include "Interpreter.h"
#include "ExpressionExecutor.h"

#include <unordered_map>

class SelectExecutor : public SubExecutorBase
{
public:
  explicit SelectExecutor(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
private:
  std::shared_ptr<ICodeNodeImplBase> searchBranches(
      const ExpressionExecutor& expression_result,
      const std::shared_ptr<ICodeNodeImplBase>& node);
  bool searchConstants(const ExpressionExecutor& expression_result,
                       const std::shared_ptr<ICodeNodeImplBase>& branch_node);
};

class SelectExecutorOpt : public SubExecutorBase
{
public:
  explicit SelectExecutorOpt(const std::shared_ptr<Executor>& executor);
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node);
private:
  typedef std::shared_ptr<ICodeNodeImplBase> NodeT;
  typedef std::unordered_map<VariableValueT, NodeT> JumpTableT;
  std::unordered_map<NodeT, JumpTableT> mJumpCache;
};

#endif // SELECTEXECUTOR_H
