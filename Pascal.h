#ifndef PASCAL_H
#define PASCAL_H

#include "Backend.h"
#include "Frontend.h"
#include "Intermediate.h"
#include "PascalFrontend.h"
#include "Common.h"

#include <QFile>
#include <QObject>

class Pascal: public QObject {
  Q_OBJECT
public:
  Pascal(const QString& operation, const QString& filePath,
         const QString& flags, QObject* parent = nullptr);
  ~Pascal();
public slots:
  void sourceMessage(int lineNumber, QString line);
  void parserSummary(int lineNumber, int errorCount, float elapsedTime);
  void compilerSummary(int instructionCount, float elapsedTime);
  void interpreterSummary(int executionCount, int runtimeErrors, float elapsedTime);
  void tokenMessage(int lineNumber, int position, PascalTokenType tokenType,
                    QString text, QVariant value);
  void syntaxErrorMessage(int lineNumber, int position, QString text,
                          QString error);
signals:
  void exit(int exitCode);
private:
  PascalParserTopDown* mParser;
  Source* mSource;
  // do I need to hold copies here?
//  unique_ptr<ICode> mICode;
//  unique_ptr<SymbolTable> mSymbolTable;
  Backend* mBackend;
  QFile* mSourceFile;
  QTextStream* mTextStream;
};

#endif // PASCAL_H
