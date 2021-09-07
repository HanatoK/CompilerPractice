#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"
#include "Common.h"

#include <boost/signals2.hpp>
#include <utility>
#include <memory>
#include <iostream>

class Backend;

enum class RuntimeErrorCode {
  UNINITIALIZED_VALUE,
  VALUE_RANGE,
  INVALID_CASE_EXPRESSION_VALUE,
  DIVISION_BY_ZERO,
  INVALID_STANDARD_FUNCTION_ARGUMENT,
  INVALID_INPUT,
  STACK_OVERFLOW,
  UNIMPLEMENTED_FEATURE
};

std::string runtimeErrorCodeToString(RuntimeErrorCode code);

class RuntimeErrorHandler {
public:
  RuntimeErrorHandler();
  void flag(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node,
            RuntimeErrorCode error_code, std::shared_ptr<Backend> backend);
private:
  static const int MAX_ERRORS = 5;
  int mErrorCount;
};

class Backend {
public:
  Backend();
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
  virtual void sendRuntimeErrorMessage(std::string msg, int line) = 0;
protected:
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mICode;
  std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> mSymbolTableStack;
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
  virtual void sendRuntimeErrorMessage(std::string msg, int line);
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
  virtual void sendRuntimeErrorMessage(std::string msg, int line);
  //  void summary(int executionCount, int runtimeErrors, float elapsedTime);
  boost::signals2::signal<void(int, int, float)> summary;
};

}

std::unique_ptr<Backend> createBackend(const std::string& operation);

#endif // BACKEND_H
