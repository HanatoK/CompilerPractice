#include "SelectExecutor.h"
#include "ExpressionExecutor.h"
#include "StatementExecutor.h"

SelectExecutor::SelectExecutor(Executor &executor): SubExecutorBase(executor)
{

}

std::shared_ptr<SubExecutorBase> SelectExecutor::execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  auto it_child = node->childrenBegin();
  auto expr_node = *it_child++;
  ExpressionExecutor expression_executor(*currentExecutor());
  expression_executor.execute(expr_node);
  auto selected_branch_node = searchBranches(expression_executor, node);
  if (selected_branch_node != nullptr) {
    StatementExecutor statement_executor(*currentExecutor());
    auto statement_node = *(selected_branch_node->childrenBegin() + 1);
    statement_executor.execute(statement_node);
  }
  ++executionCount();
  return nullptr;
}

std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > SelectExecutor::searchBranches(const ExpressionExecutor& expression_result,
    const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> >& node)
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
                                     const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> >& branch_node)
{
  auto expression_type = expression_result.valueType();
  bool integer_mode = (expression_type == VariableType::INTEGER) ? true : false;
  auto constant_node = (*branch_node->childrenBegin());
  if (integer_mode) {
    for (auto it = constant_node->childrenBegin(); it != constant_node->childrenEnd(); ++it) {
      if ((*it)->type() == ICodeNodeTypeImpl::NEGATE) {
        auto select_value = std::any_cast<long long>(expression_result.value());
        auto value_obj = (*((*it)->childrenBegin()))->getAttribute(ICodeKeyTypeImpl::VALUE);
        auto value = -std::any_cast<long long>(value_obj);
//        std::cout << "Internal compared value = " << value << " ; target value = " << select_value << std::endl;
        if (select_value == value) {
          return true;
        }
      } else {
        auto select_value = std::any_cast<long long>(expression_result.value());
        auto value_obj = (*it)->getAttribute(ICodeKeyTypeImpl::VALUE);
        auto value = std::any_cast<long long>(value_obj);
//        std::cout << "Internal compared value = " << value << " ; target value = " << select_value << std::endl;
        if (select_value == value) {
          return true;
        }
      }
    }
  } else {
    auto select_value = std::any_cast<std::string>(expression_result.value());
    for (auto it = constant_node->childrenBegin(); it != constant_node->childrenEnd(); ++it) {
      auto value_obj = (*it)->getAttribute(ICodeKeyTypeImpl::VALUE);
      auto value = std::any_cast<std::string>(value_obj);
      if (select_value == value) {
        return true;
      }
    }
  }
  return false;
}
