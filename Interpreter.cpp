#include "Interpreter.h"
#include "StatementExecutor.h"

#include <chrono>

Executor::Executor(): Backend()
{
  mErrorHandler = new RuntimeErrorHandler();
  mExecutionCount = 0;
}

Executor::~Executor()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
  if (mErrorHandler != nullptr) {
    delete mErrorHandler;
    mErrorHandler = nullptr;
  }
}

void Executor::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
    std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>
        symbol_table_stack) {
  mICode = iCode;
  mSymbolTableStack = symbol_table_stack;
  const auto start_time = std::chrono::high_resolution_clock::now();
  auto root_node = iCode->getRoot();
  StatementExecutor executor(*this);
  executor.execute(root_node);
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  summary(executionCount, runtimeErrors, elapsed_time.count());
}

SubExecutorBase::SubExecutorBase(Executor &executor): mExecutor(executor)
{

}

SubExecutorBase::~SubExecutorBase()
{

}

void SubExecutorBase::sendSourceLineMessage(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > node)
{
  const auto line_number_obj = node->getAttribute(ICodeKeyTypeImpl::LINE);
  if (line_number_obj.has_value()) {
    const int line_number = std::any_cast<int>(line_number_obj);
    mExecutor.sourceLineMessage(line_number);
  }
}

Executor *SubExecutorBase::currentExecutor()
{
  return &mExecutor;
}

RuntimeErrorHandler *SubExecutorBase::errorHandler()
{
  return mExecutor.mErrorHandler;
}

int &SubExecutorBase::executionCount()
{
  return mExecutor.mExecutionCount;
}
