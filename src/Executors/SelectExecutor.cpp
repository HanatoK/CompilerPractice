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
  // is there already an entry for this SELECT node in the jump table cache?
  auto search = mJumpCache.find(node);
  if (search == mJumpCache.end()) {
    // if not, create a new one
    mJumpCache[node] = createJumpTable(node);
  }
  // get the SELECT node's children
  const auto it_select_child = node->childrenBegin();
  auto it_expr_node = (*it_select_child)->childrenBegin();
  // evaluate the SELECT expression
  ExpressionExecutor expression_executor(currentExecutor());
  expression_executor.execute(*it_expr_node);
  auto select_value = expression_executor.value();
  auto jump_entry = mJumpCache.at(node);
  auto search_branch = jump_entry.find(select_value);
  if (search_branch != jump_entry.end()) {
    StatementExecutor statement_executor(currentExecutor());
    statement_executor.execute(search_branch->second);
  }
  ++executionCount();
  return nullptr;
}

SelectExecutorOpt::JumpTableT SelectExecutorOpt::createJumpTable(SelectExecutorOpt::NodeT node) const {
  JumpTableT jump_table;
  // loop over children that are SELECT_BRANCH nodes
  for (auto children_it = node->childrenBegin();
       children_it != node->childrenEnd(); ++children_it) {
    auto branch_node = *children_it;
    auto constant_node = *(branch_node->childrenBegin());
    auto statement_node = *(std::next(branch_node->childrenBegin()));
    // loop over the constants children of the branch's CONSTANTS_NODE
    for (auto constants_list_it = constant_node->childrenBegin();
         constants_list_it != constant_node->childrenEnd();
         ++constants_list_it) {
      // create a jump table entry
      auto value = (*constants_list_it)->getAttribute<ICodeKeyTypeImpl::VALUE>();
      if ((*constants_list_it)->type() == ICodeNodeTypeImpl::STRING_CONSTANT) {
        value = (std::any_cast<std::string>(value))[0];
      }
      jump_table.insert({value, statement_node});
    }
  }
  return jump_table;
}
