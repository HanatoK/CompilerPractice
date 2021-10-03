#ifndef COMPILER_H
#define COMPILER_H

#include "Backend.h"

class CodeGenerator: public Backend {
public:
  CodeGenerator(): Backend() {}
  virtual ~CodeGenerator() {}
  virtual void process(std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> const> iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual std::string getType() const {
    return "compiler";
  }
  //  void summary(int instructionCount, float elapsedTime);
  boost::signals2::signal<void(int, double)> summary;
};

#endif // COMPILER_H