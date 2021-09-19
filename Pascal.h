#ifndef PASCAL_H
#define PASCAL_H

#include "Backend.h"
#include "Frontend.h"
#include "Intermediate.h"
#include "PascalFrontend.h"
#include "Common.h"

#include <QFile>
#include <QObject>
#include <fstream>

class Pascal {
public:
  Pascal(const std::string& operation, const std::string& filePath,
         const std::string& flags);
  ~Pascal();
  void sourceMessage(int lineNumber, std::string line);
  void parserSummary(int lineNumber, int errorCount, float elapsedTime);
  void compilerSummary(int instructionCount, float elapsedTime);
  void interpreterSummary(int executionCount, int runtimeErrors, float elapsedTime);
  void tokenMessage(int lineNumber, int position, PascalTokenTypeImpl tokenType,
                    std::string text, std::any value);
  void syntaxErrorMessage(int lineNumber, int position, std::string text,
                          std::string error);
  void assignmentMessage(int line_number, std::string variable_name, std::any value);
  void runtimeErrorMessage(int line_number, std::string error_message);
signals:
  void exit(int exitCode);
private:
  std::shared_ptr<PascalParserTopDown> mParser;
  std::shared_ptr<Source> mSource;
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mICode;
  std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> mSymbolTableStack;
  std::shared_ptr<Backend> mBackend;
  std::ifstream mTextStream;
};

#endif // PASCAL_H
