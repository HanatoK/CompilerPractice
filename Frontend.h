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

class Scanner: public QObject {
  Q_OBJECT
public:
  Scanner(QObject* parent = nullptr);
  Scanner(Source* source, QObject* parent = nullptr);
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

class Parser: public QObject {
  Q_OBJECT
public:
  Parser(Scanner* scanner, QObject* parent = nullptr);
  virtual ~Parser();
  // parse a source program
  virtual void parse() = 0;
  virtual int errorCount() = 0;
  std::shared_ptr<Token> currentToken() const;
  std::shared_ptr<Token> nextToken();
  std::shared_ptr<SymbolTable> getSymbolTable() const;
  std::shared_ptr<ICode> getICode() const;
signals:
  void parserSummary(int lineNumber, int errorCount, float elapsedTime);
  void tokenMessage(int lineNumber, int position, QString tokenType,
                    QString text, QVariant value);
  void syntaxErrorMessage(int lineNumber, int position, QString text,
                          QString error);
protected:
  std::shared_ptr<SymbolTable> mSymbolTable;
  Scanner* mScanner;
  std::shared_ptr<ICode> mICode;
};

#endif // FRONTEND_H
