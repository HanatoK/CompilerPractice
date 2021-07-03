#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"

#include <QObject>
#include <utility>
#include <memory>

using std::unique_ptr;

class Backend: public QObject {
  Q_OBJECT
public:
  Backend(QObject* parent = nullptr);
  virtual void process(unique_ptr<ICode> iCode, unique_ptr<SymbolTable> symbolTable) = 0;
  virtual QString getType() const;
protected:
  unique_ptr<ICode> mICode;
  unique_ptr<SymbolTable> mSymbolTable;
};

namespace Compiler {

class CodeGenerator: public Backend {
  Q_OBJECT
public:
  CodeGenerator(QObject* parent = nullptr);
  virtual void process(unique_ptr<ICode> iCode, unique_ptr<SymbolTable> symbolTable);
  virtual QString getType() const;
signals:
  void summary(int instructionCount, float elapsedTime);
};

}

namespace Intepreter {

class Executor: public Backend {
  Q_OBJECT
public:
  Executor(QObject* parent = nullptr);
  virtual void process(unique_ptr<ICode> iCode, unique_ptr<SymbolTable> symbolTable);
  virtual QString getType() const;
signals:
  void summary(int executionCount, int runtimeErrors, float elapsedTime);
};

}

Backend* createBackend(const QString& operation, QObject* parent = nullptr);

#endif // BACKEND_H
