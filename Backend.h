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
  virtual ~Backend();
  virtual void process(std::shared_ptr<ICode> iCode, std::shared_ptr<SymbolTableStack> symbol_table_stack) = 0;
  virtual QString getType() const;
protected:
  std::shared_ptr<ICode> mICode;
  std::shared_ptr<SymbolTable> mSymbolTable;
};

namespace Compiler {

class CodeGenerator: public Backend {
  Q_OBJECT
public:
  CodeGenerator(QObject* parent = nullptr);
  virtual ~CodeGenerator();
  virtual void process(std::shared_ptr<ICode> iCode, std::shared_ptr<SymbolTableStack> symbol_table_stack);
  virtual QString getType() const;
signals:
  void summary(int instructionCount, float elapsedTime);
};

}

namespace Interpreter {

class Executor: public Backend {
  Q_OBJECT
public:
  Executor(QObject* parent = nullptr);
  virtual ~Executor();
  virtual void process(std::shared_ptr<ICode> iCode, std::shared_ptr<SymbolTableStack> symbol_table_stack);
  virtual QString getType() const;
signals:
  void summary(int executionCount, int runtimeErrors, float elapsedTime);
};

}

Backend* createBackend(const QString& operation, QObject* parent = nullptr);

#endif // BACKEND_H
