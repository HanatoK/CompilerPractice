#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"
#include "Common.h"

#include <boost/signals2.hpp>
#include <utility>
#include <memory>
#include <iostream>

class Backend {
public:
  Backend() {}
  virtual ~Backend() {
#ifdef DEBUG_DESTRUCTOR
    std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
  }
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack) = 0;
  virtual std::string getType() const {
    return "unknown";
  }
protected:
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mICode;
  std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>> mSymbolTable;
};

namespace Compiler {

class CodeGenerator: public Backend {
public:
  CodeGenerator(): Backend() {}
  virtual ~CodeGenerator() {}
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual std::string getType() const {
    return "compiler";
  }
  //  void summary(int instructionCount, float elapsedTime);
  boost::signals2::signal<void(int, float)> summary;
};

}

namespace Interpreter {

class Executor: public Backend {
public:
  Executor(): Backend() {}
  virtual ~Executor() {}
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual std::string getType() const {
    return "interpreter";
  }
  //  void summary(int executionCount, int runtimeErrors, float elapsedTime);
  boost::signals2::signal<void(int, int, float)> summary;
};

}

std::unique_ptr<Backend> createBackend(const std::string& operation);

#endif // BACKEND_H
