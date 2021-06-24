#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"

#include <QObject>

class Backend: public QObject {
  Q_OBJECT
public:
  Backend(QObject* parent = nullptr);
  virtual void process(ICode* iCode, SymbolTable* symbolTable) = 0;
};

namespace Compiler {

class CodeGenerator: public Backend {
  Q_OBJECT
public:
  CodeGenerator(QObject* parent = nullptr);
};

}

#endif // BACKEND_H
