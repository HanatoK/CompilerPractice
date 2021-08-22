#include "Backend.h"

void Compiler::CodeGenerator::process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > iCode, std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) {
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int instructionCount = 0;
  emit summary(instructionCount, elapsedTime);
}

void Interpreter::Executor::process(
    std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode, std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) {
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  emit summary(executionCount, runtimeErrors, elapsedTime);
}

Backend* createBackend(const QString& operation, QObject* parent) {
  if (operation.compare("compile", Qt::CaseInsensitive) == 0) {
    return new Compiler::CodeGenerator(parent);
  } else if (operation.compare("interpret", Qt::CaseInsensitive) == 0) {
    return new Interpreter::Executor(parent);
  } else {
    return nullptr;
  }
}
