#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"
#include "Common.h"

#include <QObject>
#include <utility>
#include <memory>
#include <QTime>
#include <QDebug>

using std::unique_ptr;

class Backend: public QObject {
  Q_OBJECT
public:
  Backend(QObject* parent = nullptr): QObject(parent) {}
  virtual ~Backend() {
#ifdef DEBUG_DESTRUCTOR
    qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
  }
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) = 0;
  virtual QString getType() const {
    return "unknown";
  }
protected:
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mICode;
  std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>> mSymbolTable;
};

namespace Compiler {

class CodeGenerator: public Backend {
  Q_OBJECT
public:
  CodeGenerator(QObject* parent = nullptr): Backend(parent) {}
  virtual ~CodeGenerator() {}
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual QString getType() const {
    return "compiler";
  }
signals:
  void summary(int instructionCount, float elapsedTime);
};

}

namespace Interpreter {

class Executor: public Backend {
  Q_OBJECT
public:
  Executor(QObject* parent = nullptr): Backend(parent) {}
  virtual ~Executor() {}
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual QString getType() const {
    return "interpreter";
  }
signals:
  void summary(int executionCount, int runtimeErrors, float elapsedTime);
};

}

Backend *createBackend(const QString& operation, QObject* parent = nullptr);

#endif // BACKEND_H
