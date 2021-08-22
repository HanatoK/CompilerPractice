#ifndef FRONTEND_H
#define FRONTEND_H

#include "Intermediate.h"

#include <QString>
#include <QTextStream>
#include <QObject>
#include <memory>
#include <utility>
#include <cstdio>

using std::unique_ptr;

class Scanner;
class Source;
class Token;

// the class that represents the source program
class Source: public QObject {
  Q_OBJECT
public:
  // constructor from an input stream
  Source(QTextStream& ifs, QObject* parent = nullptr);
  // return the source character of the current position
  char currentChar();
  // consume the current source character and return the next one
  char nextChar();
  // return the character following the current one with consuming
  char peekChar();
  int lineNum() const;
  int currentPos() const;
  virtual ~Source();
private:
  // read the next source line
  void readLine();
signals:
  void sendMessage(int lineNumber, QString line);
private:
  static const char EOL = '\n';
  QTextStream& mStream; // input stream of the source program
  QString mLine;        // source line
  int mLineNum;         // current source line number
  int mCurrentPos;      // current source line position
};

class Token {
public:
  Token();
  Token(Source* source);
  virtual ~Token();
  virtual QString getTypeStr() const;
  int lineNum() const;
  int position() const;
  const QVariant &value() const;
  const QString &text() const;
protected:
  // default method to extract only one-character tokens from the source
  virtual void extract();
  // Call the corresponding methods in source
  char currentChar();
  char nextChar();
  char peekChar();
protected:
  Source* mSource;
  QString mText;
  QVariant mValue;
  int mLineNum;
  int mPosition;
};

class EofToken: public Token {
public:
  EofToken();
  EofToken(Source *source);
  virtual void extract();
  virtual ~EofToken();
  virtual QString getTypeStr() const;
};

class Scanner {
public:
  Scanner();
  Scanner(Source* source);
  virtual ~Scanner();
  std::shared_ptr<Token> currentToken() const;
  virtual std::shared_ptr<Token> extractToken() = 0;
  std::shared_ptr<Token> nextToken();
  char currentChar();
  char nextChar();
protected:
  Source* mSource;
private:
  std::shared_ptr<Token> mCurrentToken;
};

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
class Parser {
public:
  Parser(std::shared_ptr<ScannerType> scanner);
  virtual ~Parser();
  virtual void parse() = 0;
  virtual int errorCount() = 0;
  std::shared_ptr<Token> currentToken() const;
  std::shared_ptr<Token> nextToken();
  std::shared_ptr<SymbolTableStack<SymbolTableKeyType>> getSymbolTableStack() const;
  std::shared_ptr<ICode<ICodeNodeType, ICodeKeyType>> getICode() const;
  std::shared_ptr<ScannerType> scanner() const;
protected:
  std::shared_ptr<SymbolTableStack<SymbolTableKeyType>> mSymbolTableStack;
  std::shared_ptr<ScannerType> mScanner;
  std::shared_ptr<ICode<ICodeNodeType, ICodeKeyType>> mICode;
};

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::Parser(std::shared_ptr<ScannerType> scanner):
  mSymbolTableStack(nullptr), mScanner(scanner), mICode(nullptr)
{
  mSymbolTableStack = createSymbolTableStack<SymbolTableKeyType>();
//  scanner->setParent(this);
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::~Parser()
{
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
std::shared_ptr<Token> Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::currentToken() const
{
  return mScanner->currentToken();
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
std::shared_ptr<Token> Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::nextToken()
{
  return mScanner->nextToken();
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
std::shared_ptr<SymbolTableStack<SymbolTableKeyType>>
Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::getSymbolTableStack() const
{
  return mSymbolTableStack;
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
std::shared_ptr<ICode<ICodeNodeType, ICodeKeyType>>
Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::getICode() const
{
  return mICode;
}

template <typename SymbolTableKeyType, typename ICodeNodeType,
          typename ICodeKeyType, typename ScannerType>
std::shared_ptr<ScannerType> Parser<SymbolTableKeyType, ICodeNodeType, ICodeKeyType, ScannerType>::scanner() const
{
  return mScanner;
}

#endif // FRONTEND_H
