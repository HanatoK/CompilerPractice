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
class Parser;
class Source;
class Token;
class TokenType;

// the class that represents the source program
class Source: public QObject {
  Q_OBJECT
public:
  // constructor from an input stream
  Source(QTextStream& ifs, QObject* parent = nullptr);
  // return the source character of the current position
  QChar currentChar();
  // consume the current source character and return the next one
  QChar nextChar();
  // return the character following the current one with consuming
  QChar peekChar();
  int lineNum() const;
  int currentPos() const;
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

class TokenType {
public:
  TokenType() {}
};

class Token {
public:
  Token();
  Token(Source* source);
  virtual ~Token();
  virtual QString getTypeStr() const;
  virtual unique_ptr<Token> clone() const;
  int lineNum() const;
  int position() const;
  const QVariant &value() const;
  const QString &text() const;
protected:
  // default method to extract only one-character tokens from the source
  virtual void extract();
  // Call the corresponding methods in source
  QChar currentChar();
  QChar nextChar();
  QChar peekChar();
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
  virtual unique_ptr<Token> clone() const;
};

class Scanner: public QObject {
  Q_OBJECT
public:
  Scanner(QObject* parent = nullptr);
  Scanner(Source* source, QObject* parent = nullptr);
  virtual ~Scanner();
  unique_ptr<Token> currentToken() const;
  virtual unique_ptr<Token> extractToken() = 0;
  unique_ptr<Token> nextToken();
  QChar currentChar();
  QChar nextChar();
protected:
  Source* mSource;
private:
  unique_ptr<Token> mCurrentToken;
};

class Parser: public QObject {
  Q_OBJECT
public:
  Parser(Scanner* scanner, QObject* parent = nullptr);
  virtual ~Parser();
  // parse a source program
  virtual void parse() = 0;
  virtual int errorCount() = 0;
  unique_ptr<Token> currentToken() const;
  unique_ptr<Token> nextToken();
  unique_ptr<SymbolTable> getSymbolTable() const;
  unique_ptr<ICode> getICode() const;
signals:
  void parserSummary(int lineNumber, int errorCount, float elapsedTime);
protected:
  unique_ptr<SymbolTable> mSymbolTable;
  Scanner* mScanner;
  unique_ptr<ICode> mICode;
};

#endif // FRONTEND_H
