#include "Interpreter.h"
#include "StatementExecutor.h"

#include <chrono>

Executor::Executor():
  Backend(), mErrorHandler(std::make_unique<RuntimeErrorHandler>()),
  mExecutionCount(0)
{
}

Executor::~Executor()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

void Executor::process(
    std::shared_ptr<ICodeImplBase const> iCode,
    std::shared_ptr<SymbolTableStackImplBase> symbol_table_stack) {
  mICode = iCode;
  mSymbolTableStack = symbol_table_stack;
  const auto start_time = std::chrono::high_resolution_clock::now();
  auto root_node = iCode->getRoot();
  StatementExecutor executor(std::dynamic_pointer_cast<std::remove_reference<decltype(*this)>::type>(shared_from_this()));
  executor.execute(root_node);
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  summary(mExecutionCount, mErrorHandler->errorCount(), elapsed_time.count());
}

SubExecutorBase::SubExecutorBase(const std::shared_ptr<Executor>& executor): mExecutor(executor)
{

}

SubExecutorBase::~SubExecutorBase()
{

}

void SubExecutorBase::sendSourceLineMessage(const std::shared_ptr<ICodeNodeImplBase>& node)
{
  const auto line_number_obj = node->getAttribute(ICodeKeyTypeImpl::LINE);
  if (line_number_obj.has_value()) {
    const int line_number = cast_by_enum<ICodeKeyTypeImpl::LINE>(line_number_obj);
    mExecutor->sourceLineMessage(line_number);
  }
}

std::shared_ptr<Executor> SubExecutorBase::currentExecutor()
{
  return mExecutor;
}

std::shared_ptr<RuntimeErrorHandler> SubExecutorBase::errorHandler()
{
  return mExecutor->mErrorHandler;
}

int &SubExecutorBase::executionCount()
{
  return mExecutor->mExecutionCount;
}
