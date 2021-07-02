#include "Backend.h"

#include <QTime>
#include <QDebug>

Backend::Backend(QObject *parent): QObject(parent)
{

}

Compiler::CodeGenerator::CodeGenerator(QObject *parent): Backend(parent)
{

}

void Compiler::CodeGenerator::process(unique_ptr<ICode> iCode, unique_ptr<SymbolTable> symbolTable)
{
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int instructionCount = 0;
  emit summary(instructionCount, elapsedTime);
}

Intepreter::Executor::Executor(QObject *parent): Backend(parent)
{

}

void Intepreter::Executor::process(unique_ptr<ICode> iCode, unique_ptr<SymbolTable> symbolTable)
{
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  emit summary(executionCount, runtimeErrors, elapsedTime);
}

Backend *createBackend(const QString &operation, QObject *parent)
{
  if (operation.compare("compile", Qt::CaseInsensitive) == 0) {
    return new Compiler::CodeGenerator(parent);
  } else if (operation.compare("execute", Qt::CaseInsensitive) == 0) {
    return new Intepreter::Executor(parent);
  } else {
    qDebug() << "Invalid operation\n";
    return nullptr;
  }
}
