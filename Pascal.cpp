#include "Pascal.h"

#include <QDebug>

Pascal::Pascal(const QString &operation, const QString &filePath,
               const QString &flags, QObject *parent):
  mParser(nullptr), mSource(nullptr), /*mICode(nullptr),mSymbolTable(nullptr),*/
  mBackend(nullptr), mSourceFile(nullptr), mTextStream(nullptr), QObject(parent)
{
  // what are these flags??
  const bool intermediate = flags.indexOf('i') > -1;
  const bool xref = flags.indexOf('x') > -1;
  mSourceFile = new QFile(filePath, parent);
  mSourceFile->open(QIODevice::ReadOnly);
  mTextStream = new QTextStream(mSourceFile);
  mSource = new Source(*mTextStream, parent);
  mParser = createParser("Pascal", "top-down", mSource, parent);
  connect(mSource, &Source::sendMessage, this, &Pascal::sourceMessageReceived);
  connect(mParser, &Parser::parserSummary, this, &Pascal::parserMessageReceived);
  mBackend = createBackend(operation, parent);
  const QString backend_type = mBackend->getType();
  if (backend_type.compare("compiler", Qt::CaseInsensitive) == 0) {
    connect(dynamic_cast<Compiler::CodeGenerator*>(mBackend),
            &Compiler::CodeGenerator::summary, this, &Pascal::compilerMessageReceived);
  } else if (backend_type.compare("interpreter", Qt::CaseInsensitive) == 0) {
    connect(dynamic_cast<Interpreter::Executor*>(mBackend),
            &Interpreter::Executor::summary, this, &Pascal::interpreterMessageReceived);
  }
  mParser->parse();
//  mICode = mParser->getICode();
//  mSymbolTable = mParser->getSymbolTable();
  mBackend->process(mParser->getICode(), mParser->getSymbolTable());
}

Pascal::~Pascal()
{
//  if (mParser != nullptr) {
//    delete mParser;
//    mParser = nullptr;
//  }
//  if (mSource != nullptr) {
//    delete mSource;
//    mSource = nullptr;
//  }
//  if (mBackend != nullptr) {
//    delete mBackend;
//    mBackend = nullptr;
//  }
  if (mTextStream != nullptr) {
    delete mTextStream;
    mTextStream = nullptr;
  }
//  if (mSourceFile != nullptr) {
//    if (mSourceFile->isOpen()) mSourceFile->close();
//    delete mSourceFile;
//    mSourceFile = nullptr;
//  }
}

void Pascal::sourceMessageReceived(int lineNumber, QString line)
{
  qInfo().noquote() << QString("%1 %2").arg(lineNumber, 3, 10, QChar('0')).arg(line);
}

void Pascal::parserMessageReceived(int lineNumber, int errorCount, float elapsedTime)
{
  const QString format = QString("\n%1 source lines.") +
                         QString("\n%2 syntax errors.") +
                         QString("\n%3 seconds total parsing time.\n");
  qInfo().noquote() << format.arg(lineNumber, 10, 10).
                              arg(errorCount, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}

void Pascal::compilerMessageReceived(int instructionCount, float elapsedTime)
{
  const QString format = QString("\n%1 instructions generated.") +
                         QString("\n%2 seconds total code generation time.\n");
  qInfo().noquote() << format.arg(instructionCount, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}

void Pascal::interpreterMessageReceived(int executionCount, int runtimeErrors, float elapsedTime)
{
  const QString format = QString("\n%1 statements executed.") +
                         QString("\n%2 runtime errors.") +
                         QString("\n%3 seconds total execution time.\n");
  qInfo().noquote() << format.arg(executionCount, 10, 10).
                              arg(runtimeErrors, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}
