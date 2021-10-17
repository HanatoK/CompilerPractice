#include "LoopExecutor.h"
#include "ExpressionExecutor.h"
#include "StatementExecutor.h"

LoopExecutor::LoopExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> LoopExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  bool exit_loop = false;
  std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> expr_node = nullptr;
  ExpressionExecutor expression_executor(*currentExecutor());
  StatementExecutor statement_executor(*currentExecutor());
  while (!exit_loop) {
    ++executionCount();
    // execute the children of the LOOP node
    for (auto it_loop_children = node->childrenBegin();
         it_loop_children != node->childrenEnd(); ++it_loop_children) {
      const auto child_type = (*it_loop_children)->type();
      if (child_type == ICodeNodeTypeImpl::TEST) {
        if (expr_node == nullptr) {
          expr_node = *((*it_loop_children)->childrenBegin());
        }
        expression_executor.execute(expr_node);
        if (!std::holds_alternative<bool>(expression_executor.value())) {
          std::cerr << "BUG: LoopExecutor::execute does not execute an expression returning a boolean value!" << std::endl;
        }
        exit_loop = std::get<bool>(expression_executor.value());
      } else {
        // statement node
        statement_executor.execute(*it_loop_children);
      }
      if (exit_loop) {
        break;
      }
    }
  }
  return nullptr;
}
