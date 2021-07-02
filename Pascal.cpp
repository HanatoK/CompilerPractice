#include "Pascal.h"

#include <QDebug>

Pascal::Pascal(const QString &operation, const QString &filePath,
               const QString &flags, QObject *parent):
  mParser(nullptr), mSource(nullptr), /*mICode(nullptr),mSymbolTable(nullptr),*/
  mBackend(nullptr), mSourceFile(nullptr), mTextStream(nullptr), QObject(parent)
{
  const bool intermediate = flags.indexOf('i') > -1;
  const bool xref = flags.indexOf('x') > -1;
  mSourceFile = new QFile(filePath, parent);
  mTextStream = new QTextStream(mSourceFile);
  mSource = new Source(*mTextStream, parent);
  mParser = createParser("Pascal", "top-down", mSource, parent);
  mBackend = createBackend(operation, parent);
  connect(mSource, &Source::sendMessage, this, &Pascal::sourceMessageReceived);
  connect(mParser, &Parser::sendMessage, this, &Pascal::parserMessageReceived);
  // TODO: connect backend message sender to receiver
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
  qInfo().noquote() << format.arg(lineNumber, 3, 10, QChar('0')).
                              arg(errorCount, 3, 10, QChar('0')).
                              arg(elapsedTime, 10, 'g', 2);
}
