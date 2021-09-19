#ifndef PASCALFRONTEND_H
#define PASCALFRONTEND_H

#include "Frontend.h"
#include "Intermediate.h"
#include "Common.h"

#include <boost/signals2/connection.hpp>
#include <map>
#include <set>

class PascalErrorHandler;
class PascalSubparserTopDownBase;

typedef Token<PascalTokenTypeImpl> PascalToken;

class PascalEofToken: public EofToken<PascalTokenTypeImpl> {
public:
  PascalEofToken();
  PascalEofToken(std::shared_ptr<Source> source);
};

std::string typeToStr(const PascalTokenTypeImpl &tokenType, bool *ok = nullptr);

class PascalScanner: public Scanner<PascalTokenTypeImpl> {
public:
  PascalScanner();
  PascalScanner(std::shared_ptr<Source> source);
  virtual ~PascalScanner();
  virtual std::shared_ptr<PascalToken> extractToken();
private:
  void skipWhiteSpace();
};

class PascalParserTopDown:
  public Parser<SymbolTableKeyTypeImpl, ICodeNodeTypeImpl,
                ICodeKeyTypeImpl, PascalTokenTypeImpl, PascalScanner> {
public:
  PascalParserTopDown(std::shared_ptr<PascalScanner> scanner);
  virtual ~PascalParserTopDown();
  virtual void parse();
  virtual int errorCount();
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  boost::signals2::signal<void(int, int, PascalTokenTypeImpl, std::string, std::any)> pascalTokenMessage;
  boost::signals2::signal<void(int, int, float)> parserSummary;
  boost::signals2::signal<void(int, int, std::string, std::string, std::any)> tokenMessage;
  boost::signals2::signal<void(int, int, std::string, std::string)> syntaxErrorMessage;
  friend class PascalSubparserTopDownBase;
protected:
  PascalErrorHandler* mErrorHandler;
};

class PascalSubparserTopDownBase {
public:
  explicit PascalSubparserTopDownBase(PascalParserTopDown& pascal_parser);
  virtual ~PascalSubparserTopDownBase();
  std::shared_ptr<PascalToken> currentToken() const;
  std::shared_ptr<PascalToken> nextToken();
  std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> getSymbolTableStack();
  std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> getICode() const;
  std::shared_ptr<PascalScanner> scanner() const;
  int errorCount();
  PascalErrorHandler* errorHandler();
  PascalParserTopDown* currentParser();
protected:
  std::set<PascalTokenTypeImpl> mStatementStartSet;
  std::set<PascalTokenTypeImpl> mStatementFollowSet;
private:
  PascalParserTopDown& mPascalParser;
};

class PascalErrorHandler {
public:
  PascalErrorHandler();
  virtual ~PascalErrorHandler();
  void flag(const std::shared_ptr<PascalToken> &token, PascalErrorCode errorCode, PascalParserTopDown *parser);
  void abortTranslation(PascalErrorCode errorCode, PascalParserTopDown *parser);
  int errorCount() const;
private:
  static const int maxError = 25;
  static std::map<PascalErrorCode, std::string> errorMessageMap;
  int mErrorCount;
};

class PascalErrorToken: public PascalToken {
public:
  PascalErrorToken();
  PascalErrorToken(std::shared_ptr<Source> source, PascalErrorCode errorCode,
                   const std::string& tokenText);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalWordToken: public PascalToken {
public:
  PascalWordToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalStringToken: public PascalToken {
public:
  PascalStringToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalSpecialSymbolToken: public PascalToken {
public:
  PascalSpecialSymbolToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalNumberToken: public PascalToken {
public:
  PascalNumberToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
  virtual void extractNumber(std::string& text);
private:
  std::string unsignedIntegerDigits(std::string& text);
  unsigned long long computeIntegerValue(std::string& digits);
  double computeFloatValue(std::string& whole_digits, std::string& fraction_digits,
                           std::string& exponent_digits, char exponent_sign);
};

std::unique_ptr<PascalParserTopDown> createPascalParser(const std::string& language, const std::string& type,
  std::shared_ptr<Source> source);

#endif // PASCALFRONTEND_H
