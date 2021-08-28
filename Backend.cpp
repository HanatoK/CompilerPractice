#include "Backend.h"

void Compiler::CodeGenerator::process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > iCode, std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) {
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int instructionCount = 0;
  summary(instructionCount, elapsedTime);
}

void Interpreter::Executor::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode, std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) {
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  summary(executionCount, runtimeErrors, elapsedTime);
}

std::unique_ptr<Backend> createBackend(const std::string& operation) {
  if (boost::iequals(operation, "compile")) {
    return std::make_unique<Compiler::CodeGenerator>();
  } else if (boost::iequals(operation, "interpret")) {
    return std::make_unique<Interpreter::Executor>();
  } else {
    return nullptr;
  }
}
