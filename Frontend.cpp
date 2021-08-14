#include "Frontend.h"

#include <QDebug>
#include <QTime>

Source::Source(QTextStream &ifs, QObject *parent): mStream(ifs), QObject(parent)
{
  mLineNum = 0;
  mCurrentPos = -2;
}

char Source::currentChar()
{
  if (mCurrentPos == -2) {
    // first time?
    readLine();
    return nextChar();
  } else if (mStream.atEnd()) {
    // at the end of file?
    return EOF;
  } else if ((mCurrentPos == -1) || (mCurrentPos == mLine.size())) {
    // at the end of line?
    return EOL;
  } else if (mCurrentPos > mLine.length()) {
    // need to read the next line?
    readLine();
    return nextChar();
  } else {
    return mLine.at(mCurrentPos).toLatin1();
  }
}

char Source::nextChar()
{
  ++mCurrentPos;
  return currentChar();
}

char Source::peekChar()
{
  currentChar();
  if (mStream.atEnd()) {
    return EOF;
  }
  const int nextPos = mCurrentPos + 1;
  if (nextPos < mLine.size()) {
    return mLine.at(nextPos).toLatin1();
  } else {
    return EOL;
  }
}

void Source::readLine()
{
  const bool read_ok = mStream.readLineInto(&mLine);
  mCurrentPos = -1;
  if (read_ok) {
    // not EOF or error
    ++mLineNum;
    emit sendMessage(mLineNum, mLine);
  }
}

int Source::currentPos() const
{
  return mCurrentPos;
}

Source::~Source()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

int Source::lineNum() const
{
  return mLineNum;
}

Token::Token(): mSource(nullptr)
{
  mLineNum = -1;
  mPosition = -2;
}

Token::Token(Source* source): mSource(source)
{
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  extract();
}

Token::~Token()
{
//#ifdef DEBUG_DESTRUCTOR
//  qDebug() << "Destructor: " << Q_FUNC_INFO;
//#endif
}

QString Token::getTypeStr() const
{
  return "unknown";
}

void Token::extract()
{
  mText = QString{currentChar()};
  mValue = QVariant();
  nextChar();
}

char Token::currentChar()
{
  return mSource->currentChar();
}

char Token::nextChar()
{
  return mSource->nextChar();
}

char Token::peekChar()
{
  return mSource->peekChar();
}

const QString &Token::text() const
{
  return mText;
}

const QVariant &Token::value() const
{
  return mValue;
}

int Token::position() const
{
  return mPosition;
}

int Token::lineNum() const
{
  return mLineNum;
}

Scanner::Scanner(QObject *parent):
  QObject(parent), mSource(nullptr), mCurrentToken(nullptr)
{

}

Scanner::Scanner(Source *source, QObject *parent):
  QObject(parent), mSource(source), mCurrentToken(nullptr)
{

}

Scanner::~Scanner()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::shared_ptr<Token> Scanner::currentToken() const
{
  return mCurrentToken;
}

std::shared_ptr<Token> Scanner::nextToken()
{
  mCurrentToken = extractToken();
  return currentToken();
}

char Scanner::currentChar()
{
  return mSource->currentChar();
}

char Scanner::nextChar()
{
  return mSource->nextChar();
}

Parser::Parser(Scanner *scanner, QObject *parent):
  QObject(parent), mSymbolTableStack(nullptr), mScanner(scanner), mICode(nullptr)
{
  mSymbolTableStack = createSymbolTableStack();
  scanner->setParent(this);
}

Parser::~Parser()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::shared_ptr<Token> Parser::currentToken() const
{
  return mScanner->currentToken();
}

std::shared_ptr<Token> Parser::nextToken()
{
  return mScanner->nextToken();
}

std::shared_ptr<SymbolTableStack> Parser::getSymbolTableStack() const
{
  return mSymbolTableStack;
}

std::shared_ptr<ICode> Parser::getICode() const
{
  return mICode;
}

EofToken::EofToken()
{

}

EofToken::EofToken(Source *source)
{
  mSource = source;
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  EofToken::extract();
}

void EofToken::extract()
{

}

EofToken::~EofToken()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

QString EofToken::getTypeStr() const
{
  return "EOF";
}
