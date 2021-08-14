#ifndef PASCALFRONTEND_H
#define PASCALFRONTEND_H

#include "Frontend.h"

#include <map>

class PascalErrorHandler;

class PascalScanner: public Scanner {
  Q_OBJECT
public:
  PascalScanner(QObject* parent = nullptr);
  PascalScanner(Source* source, QObject* parent = nullptr);
  virtual ~PascalScanner();
  virtual std::shared_ptr<Token> extractToken();
private:
  void skipWhiteSpace();
};

class PascalToken;

enum class PascalTokenType {
  // reserved words
  AND, ARRAY, BEGIN, CASE, CONST, DIV, DO, DOWNTO, ELSE, END,
  FILE, FOR, FUNCTION, GOTO, IF, IN, LABEL, MOD, NIL, NOT,
  OF, OR, PACKED, PROCEDURE, PROGRAM, RECORD, REPEAT, SET,
  THEN, TO, TYPE, UNTIL, VAR, WHILE, WITH, IDENTIFIER, INTEGER,
  REAL, STRING, ERROR, END_OF_FILE,
  // special symbols
  PLUS, MINUS, STAR, SLASH, COLON_EQUALS, DOT, COMMA, SEMICOLON,
  COLON, QUOTE, EQUALS, NOT_EQUALS, LESS_THAN, LESS_EQUALS,
  GREATER_EQUALS, GREATER_THAN, LEFT_PAREN, RIGHT_PAREN,
  LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,
  UP_ARROW, DOT_DOT
};

class PascalToken: public Token {
public:
  PascalToken();
  PascalToken(Source* source);
  virtual QString getTypeStr() const;
  static QString typeToStr(const PascalTokenType& tokenType, bool* ok = nullptr);
  static PascalTokenType strToType(const QString& str, bool* ok = nullptr);
  // maybe I need boost::bimap
  static std::map<PascalTokenType, QString> mReservedWordsMap;
  static std::map<QString, PascalTokenType> mReservedWordsMapRev;
  static std::map<PascalTokenType, QString> mSpecialSymbolsMap;
  static std::map<QString, PascalTokenType> mSpecialSymbolsMapRev;
  static std::map<PascalTokenType, QString> mSpecialWordsMap;
  static std::map<QString, PascalTokenType> mSpecialWordsMapRev;
  PascalTokenType type() const;
protected:
  PascalTokenType mType;
};

class PascalParserTopDown: public Parser {
  Q_OBJECT
public:
  PascalParserTopDown(Scanner* scanner, QObject* parent = nullptr);
  virtual ~PascalParserTopDown();
  virtual void parse();
  virtual int errorCount();
signals:
  void pascalTokenMessage(int lineNumber, int position, PascalTokenType tokenType,
                          QString text, QVariant value);
protected:
  PascalErrorHandler* mErrorHandler;
};

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

class PascalErrorHandler: public QObject {
  Q_OBJECT
public:
  PascalErrorHandler(QObject* parent = nullptr);
  virtual ~PascalErrorHandler();
  void flag(const std::shared_ptr<Token>& token, PascalErrorCode errorCode, Parser* parser);
  void abortTranslation(PascalErrorCode errorCode, Parser* parser);
  int errorCount() const;
private:
  static const int maxError = 25;
  static std::map<PascalErrorCode, QString> errorMessageMap;
  int mErrorCount;
};

class PascalErrorToken: public PascalToken {
public:
  PascalErrorToken();
  PascalErrorToken(Source *source, PascalErrorCode errorCode,
                   const QString& tokenText);
  virtual unique_ptr<Token> clone() const;
  virtual void extract();
};

class PascalWordToken: public PascalToken {
public:
  PascalWordToken(Source *source);
  virtual unique_ptr<Token> clone() const;
  virtual void extract();
};

class PascalStringToken: public PascalToken {
public:
  PascalStringToken(Source *source);
  virtual unique_ptr<Token> clone() const;
  virtual void extract();
};

class PascalSpecialSymbolToken: public PascalToken {
public:
  PascalSpecialSymbolToken(Source *source);
  virtual unique_ptr<Token> clone() const;
  virtual void extract();
};

class PascalNumberToken: public PascalToken {
public:
  PascalNumberToken(Source *source);
  virtual unique_ptr<Token> clone() const;
  virtual void extract();
  virtual void extractNumber(QString& text);
private:
  QString unsignedIntegerDigits(QString& text);
  qulonglong computeIntegerValue(QString& digits);
  double computeFloatValue(QString& whole_digits, QString& fraction_digits,
                           QString& exponent_digits, char exponent_sign);
};

PascalParserTopDown* createPascalParser(
  const QString& language, const QString& type,
  Source* source, QObject* parent = nullptr);

#endif // PASCALFRONTEND_H
