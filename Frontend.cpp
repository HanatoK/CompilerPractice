#include "Frontend.h"

#include <QDebug>
#include <QTime>

Source::Source(QTextStream &ifs, QObject *parent): mStream(ifs), QObject(parent)
{
  mLineNum = 0;
  mCurrentPos = -2;
}

QChar Source::currentChar()
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
    return mLine.at(mCurrentPos);
  }
}

QChar Source::nextChar()
{
  ++mCurrentPos;
  return currentChar();
}

QChar Source::peekChar()
{
  currentChar();
  if (mStream.atEnd()) {
    return EOF;
  }
  const int nextPos = mCurrentPos + 1;
  if (nextPos < mLine.size()) {
    return mLine.at(nextPos);
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

}

QString Token::getTypeStr() const
{
  return "unknown";
}

unique_ptr<Token> Token::clone() const
{
  return std::make_unique<Token>(*this);
}

void Token::extract()
{
  mText = QString(currentChar());
  mValue = QVariant();
  nextChar();
}

QChar Token::currentChar()
{
  return mSource->currentChar();
}

QChar Token::nextChar()
{
  return mSource->nextChar();
}

QChar Token::peekChar()
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

}

unique_ptr<Token> Scanner::currentToken() const
{
  return mCurrentToken->clone();
}

unique_ptr<Token> Scanner::nextToken()
{
  mCurrentToken = extractToken();
  return currentToken();
}

QChar Scanner::currentChar()
{
  return mSource->currentChar();
}

QChar Scanner::nextChar()
{
  return mSource->nextChar();
}

Parser::Parser(Scanner *scanner, QObject *parent):
  QObject(parent), mSymbolTable(nullptr), mScanner(scanner), mICode(nullptr)
{
  scanner->setParent(this);
}

Parser::~Parser()
{

}

unique_ptr<Token> Parser::currentToken() const
{
  return mScanner->currentToken();
}

unique_ptr<Token> Parser::nextToken()
{
  return mScanner->nextToken();
}

unique_ptr<SymbolTable> Parser::getSymbolTable() const
{
  return std::make_unique<SymbolTable>(*mSymbolTable);
}

unique_ptr<ICode> Parser::getICode() const
{
  return std::make_unique<ICode>(*mICode);
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

}

QString EofToken::getTypeStr() const
{
  return "EOF";
}

unique_ptr<Token> EofToken::clone() const
{
  return std::make_unique<EofToken>(*this);
}
