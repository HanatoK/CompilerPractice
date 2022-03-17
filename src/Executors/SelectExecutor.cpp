#include "SelectExecutor.h"
#include "ExpressionExecutor.h"
#include "StatementExecutor.h"

SelectExecutor::SelectExecutor(const std::shared_ptr<Executor> &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> SelectExecutor::execute(const std::shared_ptr<ICodeNodeImplBase> &node)
{
  auto it_child = node->childrenBegin();
  auto expr_node = *it_child++;
  ExpressionExecutor expression_executor(currentExecutor());
  expression_executor.execute(expr_node);
  auto selected_branch_node = searchBranches(expression_executor, node);
  if (selected_branch_node != nullptr) {
    StatementExecutor statement_executor(currentExecutor());
    auto statement_node = *(selected_branch_node->childrenBegin() + 1);
    statement_executor.execute(statement_node);
  }
  ++executionCount();
  return nullptr;
}

std::shared_ptr<ICodeNodeImplBase> SelectExecutor::searchBranches(const ExpressionExecutor& expression_result,
    const std::shared_ptr<ICodeNodeImplBase>& node)
{
  auto it_child = node->childrenBegin();
  for (auto it = it_child + 1; it != node->childrenEnd(); ++it) {
    auto branch_node = *it;
    if (searchConstants(expression_result, branch_node)) {
      return branch_node;
    }
  }
  return nullptr;
}

bool SelectExecutor::searchConstants(const ExpressionExecutor& expression_result,
                                     const std::shared_ptr<ICodeNodeImplBase>& branch_node)
{
  const auto constant_node = (*branch_node->childrenBegin());
  const auto select_value = expression_result.value();
  ExpressionExecutor constant_expression_executor(currentExecutor());
  for (auto it = constant_node->childrenBegin(); it != constant_node->childrenEnd(); ++it) {
    constant_expression_executor.execute(*it);
    if (constant_expression_executor.value() == select_value) {
      return true;
    }
  }
  return false;
}

SelectExecutorOpt::SelectExecutorOpt(const std::shared_ptr<Executor>& executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> SelectExecutorOpt::execute(const std::shared_ptr<ICodeNodeImplBase>& node)
{
  const auto it_select_child = node->childrenBegin();
  if (mJumpCache.find(node) == mJumpCache.end()) {
    ExpressionExecutor constant_expression_executor(currentExecutor());
    for (auto it = it_select_child + 1; it != node->childrenEnd(); ++it) {
      const auto branch_node = *it;
      const auto constant_node = *(branch_node->childrenBegin());
      const auto statement_node = *(branch_node->childrenBegin() + 1);
      for (auto it_constant_node = constant_node->childrenBegin();
           it_constant_node != constant_node->childrenEnd();
           ++it_constant_node) {
        constant_expression_executor.execute(*it_constant_node);
        auto value = constant_expression_executor.value();
        mJumpCache[node][value] = statement_node;
      }
    }
  }
//  auto it_select_child = node->childrenBegin();
  auto expr_node = *it_select_child;
  ExpressionExecutor expression_executor(currentExecutor());
  expression_executor.execute(expr_node);
  auto select_value = expression_executor.value();
  auto jump_entry = mJumpCache.at(node);
  if (jump_entry.find(select_value) != jump_entry.end()) {
    StatementExecutor statement_executor(currentExecutor());
    statement_executor.execute(jump_entry.at(select_value));
  }
  ++executionCount();
  return nullptr;
}
