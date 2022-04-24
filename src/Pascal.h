#ifndef PASCAL_H
#define PASCAL_H

#include "Backend.h"
#include "Frontend.h"
#include "Intermediate.h"
#include "PascalFrontend.h"
#include "Predefined.h"
#include "Common.h"

#include <fstream>

class Pascal {
public:
  Pascal(const std::string& operation, const std::string& filePath,
         const std::string& flags);
  ~Pascal();
  void sourceMessage(int lineNumber, const std::string& line) const;
  void parserSummary(int lineNumber, int errorCount, float elapsedTime) const;
  void compilerSummary(int instructionCount, float elapsedTime) const;
  void interpreterSummary(int executionCount, int runtimeErrors, float elapsedTime) const;
  void syntaxErrorMessage(int lineNumber, int position, const std::string& text,
                          const std::string& error) const;
  void assignmentMessage(int line_number, const std::string& variable_name, const VariableValueT& value) const;
  void runtimeErrorMessage(int line_number, const std::string& error_message) const;
private:
  std::shared_ptr<PascalParserTopDown> mParser;
  std::shared_ptr<Source> mSource;
  std::shared_ptr<ICodeImplBase> mICode;
  std::shared_ptr<SymbolTableStackImplBase> mSymbolTableStack;
  std::shared_ptr<Backend> mBackend;
  std::shared_ptr<std::ifstream> mTextStream;
};

#endif // PASCAL_H
