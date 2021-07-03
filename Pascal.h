#ifndef PASCAL_H
#define PASCAL_H

#include "Backend.h"
#include "Frontend.h"
#include "Intermediate.h"

#include <QFile>
#include <QObject>

class Pascal: public QObject {
  Q_OBJECT
public:
  Pascal(const QString& operation, const QString& filePath,
         const QString& flags, QObject* parent = nullptr);
  ~Pascal();
public slots:
  void sourceMessageReceived(int lineNumber, QString line);
  void parserMessageReceived(int lineNumber, int errorCount, float elapsedTime);
  void compilerMessageReceived(int instructionCount, float elapsedTime);
  void intepreterMessageReceived(int executionCount, int runtimeErrors, float elapsedTime);
private:
  Parser* mParser;
  Source* mSource;
  // do I need to hold copies here?
//  unique_ptr<ICode> mICode;
//  unique_ptr<SymbolTable> mSymbolTable;
  Backend* mBackend;
  QFile* mSourceFile;
  QTextStream* mTextStream;
};

#endif // PASCAL_H
