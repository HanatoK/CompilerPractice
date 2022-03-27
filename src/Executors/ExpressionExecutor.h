#ifndef EXPRESSIONEXECUTOR_H
#define EXPRESSIONEXECUTOR_H

#include "Interpreter.h"

#include <set>
#include <variant>

class ExpressionExecutor : public SubExecutorBase {
public:
  explicit ExpressionExecutor(const std::shared_ptr<Executor>& executor);
  std::shared_ptr<SubExecutorBase>
  execute(const std::shared_ptr<ICodeNodeImplBase>& node) override;
  [[nodiscard]] VariableValueT value() const;
//  VariableInternalType valueType() const;

private:
  static const std::set<ICodeNodeTypeImpl> mArithOps;
  VariableValueT mValue;
//  VariableInternalType mValueType;
  std::shared_ptr<SubExecutorBase> executeBinaryOperator(
      const std::shared_ptr<ICodeNodeImplBase> &node,
      ICodeNodeTypeImpl node_type);
};

#endif // EXPRESSIONEXECUTOR_H
