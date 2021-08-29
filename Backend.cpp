#include "Backend.h"

#include <chrono>

void Compiler::CodeGenerator::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
    std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>
        symbol_table_stack) {
  const auto start_time = std::chrono::high_resolution_clock::now();
  // do something
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  const int instructionCount = 0;
  summary(instructionCount, elapsed_time.count());
}

void Interpreter::Executor::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
    std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>
        symbol_table_stack) {
  const auto start_time = std::chrono::high_resolution_clock::now();
  // do something
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  summary(executionCount, runtimeErrors, elapsed_time.count());
}

std::unique_ptr<Backend> createBackend(const std::string &operation) {
  if (boost::iequals(operation, "compile")) {
    return std::make_unique<Compiler::CodeGenerator>();
  } else if (boost::iequals(operation, "interpret")) {
    return std::make_unique<Interpreter::Executor>();
  } else {
    return nullptr;
  }
}
