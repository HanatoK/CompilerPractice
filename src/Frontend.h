#ifndef FRONTEND_H
#define FRONTEND_H

#include "Intermediate.h"
#include "Common.h"
#include "Predefined.h"

#include <iostream>
#include <fstream>
#include <boost/signals2.hpp>
#include <cstdio>
#include <memory>
#include <utility>
#include <any>

using std::unique_ptr;

// class Scanner;
class Source;

// the class that represents the source program
class Source {
public:
  // constructor from an input stream
  explicit Source(std::shared_ptr<std::ifstream> ifs);
  // return the source character of the current position
  char currentChar();
  // consume the current source character and return the next one
  char nextChar();
  // return the character following the current one with consuming
  char peekChar();
  int lineNum() const;
  int currentPos() const;
  virtual ~Source();
  boost::signals2::signal<void(int, std::string)> sendMessage;

private:
  // read the next source line
  void readLine();

private:
  static const char EOL = '\n';
  std::shared_ptr<std::ifstream> mStream; // input stream of the source program
  std::string mLine;        // source line
  bool mReadOk;
  int mLineNum;         // current source line number
  int mCurrentPos;      // current source line position
};

template <typename T> class Token {
public:
  Token();
  explicit Token(std::weak_ptr<Source> source, bool use_default_extract = true);
  virtual ~Token();
  [[nodiscard]] int lineNum() const;
  [[nodiscard]] int position() const;
  [[nodiscard]] const std::any &value() const;
  [[nodiscard]] const std::string &text() const;
  [[nodiscard]] virtual bool isEof() const;
  virtual T type() const;
  virtual std::unique_ptr<Token<T>> clone() const;
  std::shared_ptr<Source> currentSource() const;
protected:
  // default method to extract only one-character tokens from the source
  virtual void extract();
  // Call the corresponding methods in source
  char currentChar();
  char nextChar();
  char peekChar();

protected:
  std::weak_ptr<Source> mSource;
  std::string mText;
  std::any mValue;
  int mLineNum;
  int mPosition;
  T mType;
};

template <typename T> Token<T>::Token() /*: mSource(nullptr)*/ {
  mLineNum = -1;
  mPosition = -2;
}

template <typename T>
Token<T>::Token(std::weak_ptr<Source> source, bool use_default_extract)
    : mSource(source) {
  mLineNum = currentSource()->lineNum();
  mPosition = currentSource()->currentPos();
  // NOTE: C++ code should call extract() explicitly in the derived class
  if (use_default_extract) {
    Token<T>::extract();
  }
}

template <typename T> Token<T>::~Token() {
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

template <typename T> void Token<T>::extract() {
  mText = std::string{currentChar()};
  mValue.reset();
  nextChar();
}

template <typename T> char Token<T>::currentChar() {
  return currentSource()->currentChar();
}

template <typename T> char Token<T>::nextChar() { return currentSource()->nextChar(); }

template <typename T> char Token<T>::peekChar() { return currentSource()->peekChar(); }

template <typename T> const std::string &Token<T>::text() const { return mText; }

template <typename T> bool Token<T>::isEof() const { return false; }

template <typename T> T Token<T>::type() const { return mType; }

template <typename T> const std::any &Token<T>::value() const { return mValue; }

template <typename T> int Token<T>::position() const { return mPosition; }

template <typename T> int Token<T>::lineNum() const { return mLineNum; }

template <typename T>
std::unique_ptr<Token<T>> Token<T>::clone() const {
  std::unique_ptr<Token<T>> result = std::make_unique<Token<T>>(*this);
  return result;
}

template<typename T>
std::shared_ptr<Source> Token<T>::currentSource() const
{
  return mSource.lock();
}

template <typename T> class EofToken : public Token<T> {
public:
  EofToken();
  explicit EofToken(std::shared_ptr<Source> source);
  virtual void extract() override;
  virtual ~EofToken();
  [[nodiscard]] virtual bool isEof() const override;
  virtual std::unique_ptr<Token<T>> clone() const override;
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
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

template <typename T> bool EofToken<T>::isEof() const { return true; }

template <typename T>
std::unique_ptr<Token<T>> EofToken<T>::clone() const {
  std::unique_ptr<Token<T>> result = std::make_unique<EofToken<T>>(*this);
  return result;
}

template <typename TokenT> class Scanner {
public:
  Scanner();
  explicit Scanner(std::shared_ptr<Source> source);
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
    : mSource(std::move(source)), mCurrentToken(nullptr) {}

template <typename TokenT> Scanner<TokenT>::~Scanner() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
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

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
class Parser {
public:
  explicit Parser(std::shared_ptr<ScannerT> scanner);
  virtual ~Parser();
  virtual void parse() = 0;
  [[nodiscard]] virtual int errorCount() const = 0;
  std::shared_ptr<Token<TokenT>> currentToken() const;
  std::shared_ptr<Token<TokenT>> nextToken();
  [[nodiscard]] std::shared_ptr<SymbolTableStackImplBase> getSymbolTableStack() const;
  [[nodiscard]] std::shared_ptr<ScannerT> scanner() const;

protected:
  std::shared_ptr<SymbolTableStackImplBase> mSymbolTableStack;
  std::shared_ptr<ScannerT> mScanner;
};

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::Parser(
    std::shared_ptr<ScannerT> scanner)
    : mSymbolTableStack(nullptr), mScanner(std::move(scanner)) {
  mSymbolTableStack = createSymbolTableStack();
  Predefined::instance(mSymbolTableStack);
  //  scanner->setParent(this);
}

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::~Parser() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<Token<TokenT>>
Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::currentToken()
    const {
  return mScanner->currentToken();
}

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<Token<TokenT>>
Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::nextToken() {
  return mScanner->nextToken();
}

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<SymbolTableStackImplBase> Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::getSymbolTableStack() const {
  return mSymbolTableStack;
}

//template <typename SymbolTableKeyT, typename DefinitionT,
//          typename TypeFormT, typename TypeKeyT,
//          typename ICodeNodeT, typename ICodeKeyT,
//          typename TokenT, typename ScannerT>
//std::shared_ptr<ICode<ICodeNodeT, ICodeKeyT>>
//Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::getICode()
//    const {
//  return mICode;
//}

template <typename SymbolTableKeyT, typename DefinitionT,
          typename TypeFormT, typename TypeKeyT,
          typename ICodeNodeT, typename ICodeKeyT,
          typename TokenT, typename ScannerT>
std::shared_ptr<ScannerT>
Parser<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, ICodeNodeT, ICodeKeyT, TokenT, ScannerT>::scanner()
    const {
  return mScanner;
}

#endif // FRONTEND_H
