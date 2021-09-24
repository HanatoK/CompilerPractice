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
  void sourceMessage(const int lineNumber, const std::string& line) const;
  void parserSummary(const int lineNumber, const int errorCount, const float elapsedTime) const;
  void compilerSummary(const int instructionCount, const float elapsedTime) const;
  void interpreterSummary(const int executionCount, const int runtimeErrors, const float elapsedTime) const;
  void tokenMessage(const int lineNumber, const int position, const PascalTokenTypeImpl tokenType,
                    const std::string& text, const std::any& value) const;
  void syntaxErrorMessage(const int lineNumber, const int position, const std::string& text,
                          const std::string& error) const;
  void assignmentMessage(const int line_number, const std::string& variable_name, const std::any& value) const;
  void runtimeErrorMessage(const int line_number, const std::string& error_message) const;
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
