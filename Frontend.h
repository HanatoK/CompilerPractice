#ifndef FRONTEND_H
#define FRONTEND_H

#include "Intermediate.h"

#include <QDebug>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <boost/signals2.hpp>
#include <cstdio>
#include <memory>
#include <utility>

using std::unique_ptr;

// class Scanner;
class Source;

// the class that represents the source program
class Source {
public:
  // constructor from an input stream
  Source(QTextStream &ifs);
  // return the source character of the current position
  char currentChar();
  // consume the current source character and return the next one
  char nextChar();
  // return the character following the current one with consuming
  char peekChar();
  int lineNum() const;
  int currentPos() const;
  virtual ~Source();
  boost::signals2::signal<void(int, QString)> sendMessage;

private:
  // read the next source line
  void readLine();

private:
  static const char EOL = '\n';
  QTextStream &mStream; // input stream of the source program
  QString mLine;        // source line
  int mLineNum;         // current source line number
  int mCurrentPos;      // current source line position
};

template <typename T> class Token {
public:
  Token();
  Token(std::shared_ptr<Source> source, bool use_default_extract = true);
  virtual ~Token();
  int lineNum() const;
  int position() const;
  const QVariant &value() const;
  const QString &text() const;
  virtual bool isEof() const;
  virtual T type() const;

protected:
  // default method to extract only one-character tokens from the source
  virtual void extract();
  // Call the corresponding methods in source
  char currentChar();
  char nextChar();
  char peekChar();

protected:
  std::shared_ptr<Source> mSource;
  QString mText;
  QVariant mValue;
  int mLineNum;
  int mPosition;
  T mType;
};

template <typename T> Token<T>::Token() : mSource(nullptr) {
  mLineNum = -1;
  mPosition = -2;
}

template <typename T>
Token<T>::Token(std::shared_ptr<Source> source, bool use_default_extract)
    : mSource(source) {
  mLineNum = mSource->lineNum();
  mPosition = mSource->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  if (use_default_extract) {
    Token<T>::extract();
  }
}

template <typename T> Token<T>::~Token() {
  //#ifdef DEBUG_DESTRUCTOR
  //  qDebug() << "Destructor: " << Q_FUNC_INFO;
  //#endif
}

template <typename T> void Token<T>::extract() {
  mText = QString{currentChar()};
  mValue = QVariant();
  nextChar();
}

template <typename T> char Token<T>::currentChar() {
  return mSource->currentChar();
}

template <typename T> char Token<T>::nextChar() { return mSource->nextChar(); }

template <typename T> char Token<T>::peekChar() { return mSource->peekChar(); }

template <typename T> const QString &Token<T>::text() const { return mText; }

template <typename T> bool Token<T>::isEof() const { return false; }

template <typename T> T Token<T>::type() const { return mType; }

template <typename T> const QVariant &Token<T>::value() const { return mValue; }

template <typename T> int Token<T>::position() const { return mPosition; }

template <typename T> int Token<T>::lineNum() const { return mLineNum; }

template <typename T> class EofToken : public Token<T> {
public:
  EofToken();
  EofToken(std::shared_ptr<Source> source);
  virtual void extract();
  virtual ~EofToken();
  virtual bool isEof() const;
};

template <typename T> EofToken<T>::EofToken() {}

template <typename T>
EofToken<T>::EofToken(std::shared_ptr<Source> source)
    : Token<T>(source, false) {
  EofToken::extract();
}

template <typename T> void EofToken<T>::extract() {}

template <typename T> EofToken<T>::~EofToken() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

template <typename T> bool EofToken<T>::isEof() const { return true; }

template <typename TokenT> class Scanner {
public:
  Scanner();
  Scanner(std::shared_ptr<Source> source);
  virtual ~Scanner();
  std::shared_ptr<Token<TokenT>> currentToken() const;
  virtual std::shared_ptr<Token<TokenT>> extractToken() = 0;
  std::shared_ptr<Token<TokenT>> nextToken();
  char currentChar();
  char nextChar();

protected:
  std::shared_ptr<Source> mSource;

private:
  std::shared_ptr<Token<TokenT>> mCurrentToken;
};

template <typename TokenT>
Scanner<TokenT>::Scanner() : mSource(nullptr), mCurrentToken(nullptr) {}

template <typename TokenT>
Scanner<TokenT>::Scanner(std::shared_ptr<Source> source)
    : mSource(source), mCurrentToken(nullptr) {}

template <typename TokenT> Scanner<TokenT>::~Scanner() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

template <typename TokenT>
std::shared_ptr<Token<TokenT>> Scanner<TokenT>::currentToken() const {
  return mCurrentToken;
}

template <typename TokenT>
std::shared_ptr<Token<TokenT>> Scanner<TokenT>::nextToken() {
  mCurrentToken = extractToken();
  return currentToken();
}

template <typename TokenT> char Scanner<TokenT>::currentChar() {
  return mSource->currentChar();
}

template <typename TokenT> char Scanner<TokenT>::nextChar() {
  return mSource->nextChar();
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
class Parser {
public:
  Parser(std::shared_ptr<ScannerT> scanner);
  virtual ~Parser();
  virtual void parse() = 0;
  virtual int errorCount() = 0;
  std::shared_ptr<Token<TokenT>> currentToken() const;
  std::shared_ptr<Token<TokenT>> nextToken();
  std::shared_ptr<SymbolTableStack<SymbolTableKeyT>>
  getSymbolTableStack() const;
  std::shared_ptr<ICode<ICodeNodeT, ICodeKeyT>> getICode() const;
  std::shared_ptr<ScannerT> scanner() const;

protected:
  std::shared_ptr<SymbolTableStack<SymbolTableKeyT>> mSymbolTableStack;
  std::shared_ptr<ScannerT> mScanner;
  std::shared_ptr<ICode<ICodeNodeT, ICodeKeyT>> mICode;
};

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::Parser(
    std::shared_ptr<ScannerT> scanner)
    : mSymbolTableStack(nullptr), mScanner(scanner), mICode(nullptr) {
  mSymbolTableStack = createSymbolTableStack<SymbolTableKeyT>();
  //  scanner->setParent(this);
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::~Parser() {
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<Token<TokenT>>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::currentToken()
    const {
  return mScanner->currentToken();
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<Token<TokenT>>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::nextToken() {
  return mScanner->nextToken();
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<SymbolTableStack<SymbolTableKeyT>>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT,
       ScannerT>::getSymbolTableStack() const {
  return mSymbolTableStack;
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<ICode<ICodeNodeT, ICodeKeyT>>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::getICode()
    const {
  return mICode;
}

template <typename SymbolTableKeyT, typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<ScannerT>
Parser<SymbolTableKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::scanner()
    const {
  return mScanner;
}

#endif // FRONTEND_H
