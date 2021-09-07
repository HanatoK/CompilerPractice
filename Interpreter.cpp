#include "Interpreter.h"

#include <chrono>

void Executor::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
    std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>
        symbol_table_stack) {
  mICode = iCode;
  mSymbolTableStack = symbol_table_stack;
  const auto start_time = std::chrono::high_resolution_clock::now();
  // do something
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  summary(executionCount, runtimeErrors, elapsed_time.count());
}

void Executor::sendRuntimeErrorMessage(std::string msg, int line)
{

}
