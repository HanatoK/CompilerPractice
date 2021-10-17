#ifndef EXPRESSIONEXECUTOR_H
#define EXPRESSIONEXECUTOR_H

#include "Interpreter.h"

#include <set>
#include <variant>

class ExpressionExecutor : public SubExecutorBase {
public:
  explicit ExpressionExecutor(Executor &executor);
  virtual std::shared_ptr<SubExecutorBase>
  execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
              &node);
  VariableValueT value() const;
  VariableType valueType() const;

private:
  static const std::set<ICodeNodeTypeImpl> mArithOps;
  VariableValueT mValue;
  VariableType mValueType;
  std::shared_ptr<SubExecutorBase> executeBinaryOperator(
      const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
          &node,
      const ICodeNodeTypeImpl node_type);
};

#endif // EXPRESSIONEXECUTOR_H
