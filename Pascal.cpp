#include "Pascal.h"

#include <QDebug>

Pascal::Pascal(const QString &operation, const QString &filePath,
               const QString &flags, QObject *parent):
  mParser(nullptr), mSource(nullptr), /*mICode(nullptr),mSymbolTable(nullptr),*/
  mBackend(nullptr), mSourceFile(nullptr), mTextStream(nullptr), QObject(parent)
{
  // what are these flags??
//  const bool intermediate = flags.indexOf('i') > -1;
//  const bool xref = flags.indexOf('x') > -1;
  mSourceFile = new QFile(filePath, this);
  mSourceFile->open(QIODevice::ReadOnly);
  mTextStream = new QTextStream(mSourceFile);
  mSource = new Source(*mTextStream, this);
  mParser = createPascalParser("Pascal", "top-down", mSource, this);
  connect(mSource, &Source::sendMessage, this, &Pascal::sourceMessage);
  connect(mParser, &PascalParserTopDown::parserSummary, this, &Pascal::parserSummary);
  connect(mParser, &PascalParserTopDown::pascalTokenMessage, this, &Pascal::tokenMessage);
  connect(mParser, &PascalParserTopDown::syntaxErrorMessage, this, &Pascal::syntaxErrorMessage);
  mBackend = createBackend(operation, this);
  const QString backend_type = mBackend->getType();
  if (backend_type.compare("compiler", Qt::CaseInsensitive) == 0) {
    connect(dynamic_cast<Compiler::CodeGenerator*>(mBackend),
            &Compiler::CodeGenerator::summary, this, &Pascal::compilerSummary);
  } else if (backend_type.compare("interpreter", Qt::CaseInsensitive) == 0) {
    connect(dynamic_cast<Interpreter::Executor*>(mBackend),
            &Interpreter::Executor::summary, this, &Pascal::interpreterSummary);
  }
  mParser->parse();
//  mICode = mParser->getICode();
//  mSymbolTable = mParser->getSymbolTable();
  mBackend->process(mParser->getICode(), mParser->getSymbolTable());
}

Pascal::~Pascal()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
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

void Pascal::sourceMessage(int lineNumber, QString line)
{
  qInfo().noquote() << QString("%1 %2").arg(lineNumber, 3, 10, QChar('0')).arg(line);
}

void Pascal::parserSummary(int lineNumber, int errorCount, float elapsedTime)
{
  const QString format = QString("\n%1 source lines.") +
                         QString("\n%2 syntax errors.") +
                         QString("\n%3 seconds total parsing time.\n");
  qInfo().noquote() << format.arg(lineNumber, 10, 10).
                              arg(errorCount, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}

void Pascal::compilerSummary(int instructionCount, float elapsedTime)
{
  const QString format = QString("\n%1 instructions generated.") +
                         QString("\n%2 seconds total code generation time.\n");
  qInfo().noquote() << format.arg(instructionCount, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}

void Pascal::interpreterSummary(int executionCount, int runtimeErrors, float elapsedTime)
{
  const QString format = QString("\n%1 statements executed.") +
                         QString("\n%2 runtime errors.") +
                         QString("\n%3 seconds total execution time.\n");
  qInfo().noquote() << format.arg(executionCount, 10, 10).
                              arg(runtimeErrors, 10, 10).
                              arg(elapsedTime, 10, 'g', 2);
}

void Pascal::tokenMessage(int lineNumber, int position, PascalTokenType tokenType, QString text, QVariant value)
{
  // TODO: invalid QVariant
  const QString format = QString(">>> %1 line = %2, pos = %3, text = %4");
  const QString type_str = PascalToken::typeToStr(tokenType);
  const QString output_msg = format.arg(type_str, 15, '-').
                             arg(lineNumber, 5, 10, QChar('0')).
                             arg(position, 3).
                             arg(text) + ", value = " + value.toString();
  qInfo().noquote() << output_msg;
}

void Pascal::syntaxErrorMessage(int lineNumber, int position, QString text, QString error)
{
  // TODO: show the correct position
  const int prefix_width = 3;
  const int space_count = prefix_width + position + 2;
  QString flag_buffer(space_count - 1, QChar(' '));
  flag_buffer += "^\n***" + error;
  flag_buffer += "[at \"" + text + "\"]";
  qInfo().noquote() << flag_buffer << '\n';
}
