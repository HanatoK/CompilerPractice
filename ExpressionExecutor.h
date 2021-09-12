#ifndef EXPRESSIONEXECUTOR_H
#define EXPRESSIONEXECUTOR_H

#include "Interpreter.h"

#include <set>

class ExpressionExecutor : public SubExecutorBase
{
public:
  explicit ExpressionExecutor(Executor &executor);
  virtual std::shared_ptr<SubExecutorBase> execute(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node);
  std::any value() const;
  VariableType valueType() const;
private:
  static const std::set<ICodeNodeTypeImpl> mArithOps;
  std::any mValue;
  VariableType mValueType;
  std::shared_ptr<SubExecutorBase> executeBinaryOperator(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node, ICodeNodeTypeImpl node_type);
};

#endif // EXPRESSIONEXECUTOR_H
