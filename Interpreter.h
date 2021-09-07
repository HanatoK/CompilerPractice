#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Backend.h"

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

#endif // INTERPRETER_H
