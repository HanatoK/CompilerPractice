#ifndef COMPILER_H
#define COMPILER_H

#include "Backend.h"
#include "IntermediateImpl.h"

class CodeGenerator: public Backend {
public:
  CodeGenerator(): Backend() {}
  virtual ~CodeGenerator() {}
  virtual void process(std::shared_ptr<const ICodeImplBase> iCode,
                       std::shared_ptr<SymbolTableStackImplBase> symbol_table_stack);
  virtual std::string getType() const {
    return "compiler";
  }
  //  void summary(int instructionCount, float elapsedTime);
  boost::signals2::signal<void(int, double)> summary;
};

#endif // COMPILER_H
