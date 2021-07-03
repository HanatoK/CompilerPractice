#ifndef PASCALFRONTEND_H
#define PASCALFRONTEND_H

#include "Frontend.h"

#include <unordered_map>

class PascalScanner: public Scanner {
  Q_OBJECT
public:
  PascalScanner(QObject* parent = nullptr);
  PascalScanner(Source* source, QObject* parent = nullptr);
  virtual ~PascalScanner();
  virtual unique_ptr<Token> extractToken();
};

class PascalParserTopDown: public Parser {
  Q_OBJECT
public:
  PascalParserTopDown(Scanner* scanner, QObject* parent = nullptr);
  virtual ~PascalParserTopDown();
  virtual void parse();
  virtual int errorCount();
signals:
  void tokenMessage(int lineNumber, int position, QString tokenType,
                    QString text, QVariant value);
};

class PascalErrorHandler: public QObject {
  Q_OBJECT
public:
  enum class PascalErrorCode {
    ALREADY_FORWARDED,
    IDENTIFIER_REDEFINED,
    IDENTIFIER_UNDEFINED,
    INCOMPATIBLE_ASSIGNMENT,
    INCOMPATIBLE_TYPES,
    INVALID_ASSIGNMENT,
    INVALID_CHARACTER,
    INVALID_CONSTANT,
    INVALID_EXPONENT,
    INVALID_EXPRESSION,
    INVALID_FIELD,
    INVALID_FRACTION,
    INVALID_IDENTIFIER_USAGE,
    INVALID_INDEX_TYPE,
    INVALID_NUMBER,
    INVALID_STATEMENT,
    INVALID_SUBRANGE_TYPE,
    INVALID_TARGET,
    INVALID_TYPE,
    INVALID_VAR_PARM,
    MIN_GT_MAX,
    MISSING_BEGIN,
    MISSING_COLON,
    MISSING_COLON_EQUALS,
    MISSING_COMMA,
    MISSING_CONSTANT,
    MISSING_DO,
    MISSING_DOT_DOT,
    MISSING_END,
    MISSING_EQUALS,
    MISSING_FOR_CONTROL,
    MISSING_IDENTIFIER,
    MISSING_LEFT_BRACKET,
    MISSING_OF,
    MISSING_PERIOD,
    MISSING_PROGRAM,
    MISSING_RIGHT_BRACKET,
    MISSING_RIGHT_PAREN,
    MISSING_SEMICOLON,
    MISSING_THEN,
    MISSING_TO_DOWNTO,
    MISSING_UNTIL,
    MISSING_VARIABLE,
    CASE_CONSTANT_REUSED,
    NOT_CONSTANT_IDENTIFIER,
    NOT_RECORD_VARIABLE,
    NOT_TYPE_IDENTIFIER,
    RANGE_INTEGER,
    RANGE_REAL,
    STACK_OVERFLOW,
    TOO_MANY_LEVELS,
    TOO_MANY_SUBSCRIPTS,
    UNEXPECTED_EOF,
    UNEXPECTED_TOKEN,
    UNIMPLEMENTED,
    UNRECOGNIZABLE,
    WRONG_NUMBER_OF_PARMS,
    IO_ERROR,
    TOO_MANY_ERRORS
  };
  PascalErrorHandler();
  virtual ~PascalErrorHandler();
  void flag(unique_ptr<Token> token, int errorCode);
  void abortTranslation(int errorCode);
private:
  static const int maxError = 25;
  std::unordered_map<PascalErrorCode, QString> errorMessageMap;
  int errorCount;
private:
  void initErrorMessageMap();
};

Parser* createParser(const QString& language, const QString& type,
                     Source* source, QObject* parent = nullptr);

#endif // PASCALFRONTEND_H
