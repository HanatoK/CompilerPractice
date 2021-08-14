#include "Backend.h"

#include <QTime>
#include <QDebug>

Backend::Backend(QObject *parent): QObject(parent)
{

}

Backend::~Backend()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

QString Backend::getType() const
{
  return "unknown";
}

Compiler::CodeGenerator::CodeGenerator(QObject *parent): Backend(parent)
{

}

Compiler::CodeGenerator::~CodeGenerator()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

void Compiler::CodeGenerator::process(std::shared_ptr<ICode> iCode, std::shared_ptr<SymbolTable> symbolTable)
{
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int instructionCount = 0;
  emit summary(instructionCount, elapsedTime);
}

QString Compiler::CodeGenerator::getType() const
{
  return "compiler";
}

Interpreter::Executor::Executor(QObject *parent): Backend(parent)
{

}

Interpreter::Executor::~Executor()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

void Interpreter::Executor::process(std::shared_ptr<ICode> iCode, std::shared_ptr<SymbolTable> symbolTable)
{
  const int startTime = QTime::currentTime().msec();
  const float elapsedTime = (QTime::currentTime().msec() - startTime) / 1000.0;
  const int executionCount = 0;
  const int runtimeErrors = 0;
  emit summary(executionCount, runtimeErrors, elapsedTime);
}

QString Interpreter::Executor::getType() const
{
  return "interpreter";
}

Backend *createBackend(const QString &operation, QObject *parent)
{
  if (operation.compare("compile", Qt::CaseInsensitive) == 0) {
    return new Compiler::CodeGenerator(parent);
  } else if (operation.compare("interpret", Qt::CaseInsensitive) == 0) {
    return new Interpreter::Executor(parent);
  } else {
    qDebug() << "Invalid operation\n";
    return nullptr;
  }
}
