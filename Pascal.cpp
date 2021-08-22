#include "Pascal.h"
#include "Utilities.h"

#include <QDebug>
#include <fmt/format.h>

Pascal::Pascal(const QString &operation, const QString &filePath,
               const QString &flags, QObject *parent):
  mParser(nullptr), mSource(nullptr), /*mICode(nullptr),mSymbolTable(nullptr),*/
  mBackend(nullptr), mSourceFile(nullptr), mTextStream(nullptr), QObject(parent)
{
  // what are these flags??
//  const bool intermediate = flags.indexOf('i') > -1;
  const bool xref = flags.indexOf('x') > -1;
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
  if (xref) {
    CrossReferencer::print(mParser->getSymbolTableStack());
  }
  mBackend->process(mParser->getICode(), mParser->getSymbolTableStack());
}

Pascal::~Pascal()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
  if (mTextStream != nullptr) {
    delete mTextStream;
    mTextStream = nullptr;
  }
  if (mSource != nullptr) {
    delete mSource;
    mSource = nullptr;
  }
  if (mParser != nullptr) {
    delete mParser;
    mParser = nullptr;
  }
}

void Pascal::sourceMessage(int lineNumber, QString line)
{
  fmt::print("{:03d} {}\n", lineNumber, line.toStdString());
}

void Pascal::parserSummary(int lineNumber, int errorCount, float elapsedTime)
{
  fmt::print("\n{:10d} source lines.", lineNumber);
  fmt::print("\n{:10d} syntax errors.", errorCount);
  fmt::print("\n{:10.5f} seconds total parsing time.\n\n", elapsedTime);
}

void Pascal::compilerSummary(int instructionCount, float elapsedTime)
{
  fmt::print("\n{:10d} instructions generated.", instructionCount);
  fmt::print("\n{:10.5f} seconds total code generation time.\n\n", elapsedTime);
}

void Pascal::interpreterSummary(int executionCount, int runtimeErrors, float elapsedTime)
{
  fmt::print("\n{:10d} statements executed.", executionCount);
  fmt::print("\n{:10d} runtime errors.", runtimeErrors);
  fmt::print("\n{:10.5f} seconds total execution time.\n\n", elapsedTime);
}

void Pascal::tokenMessage(int lineNumber, int position, PascalTokenType tokenType, QString text, QVariant value)
{
  const QString type_str = PascalToken::typeToStr(tokenType);
  fmt::print(">>> {:->15s} line = {:05d}, pos = {:3d}, text = {}",
             type_str.toStdString(), lineNumber, position, text.toStdString());
  if (!value.isNull()) {
    fmt::print(", value = {}", value.toString().toStdString());
  }
  fmt::print("\n");
}

void Pascal::syntaxErrorMessage(int lineNumber, int position, QString text, QString error)
{
  const int prefix_width = 4;
  const int space_count = prefix_width + position;
  fmt::print("{: >{}}^\n", "", space_count);
  fmt::print("***{} [at \"{}\"]\n", error.toStdString(), text.toStdString());
}
