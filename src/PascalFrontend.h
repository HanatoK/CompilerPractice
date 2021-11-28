#ifndef PASCALFRONTEND_H
#define PASCALFRONTEND_H

#include "Frontend.h"
#include "Intermediate.h"
#include "IntermediateImpl.h"
#include "Predefined.h"
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
  explicit PascalEofToken(std::shared_ptr<Source> source);
};

std::string typeToStr(const PascalTokenTypeImpl &tokenType, bool *ok = nullptr);

class PascalScanner: public Scanner<PascalTokenTypeImpl> {
public:
  PascalScanner();
  explicit PascalScanner(std::shared_ptr<Source> source);
  virtual ~PascalScanner();
  virtual std::shared_ptr<PascalToken> extractToken();
private:
  void skipWhiteSpace();
};

class PascalParserTopDown:
  public Parser<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, ICodeNodeTypeImpl,
                ICodeKeyTypeImpl, PascalTokenTypeImpl, PascalScanner> {
public:
  explicit PascalParserTopDown(std::shared_ptr<PascalScanner> scanner);
  virtual ~PascalParserTopDown();
  virtual void parse();
  virtual int errorCount() const;
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  boost::signals2::signal<void(const int, const int, const PascalTokenTypeImpl, const std::string&, std::any)> pascalTokenMessage;
  boost::signals2::signal<void(const int, const int, const float)> parserSummary;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&, const std::any&)> tokenMessage;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&)> syntaxErrorMessage;
  friend class PascalSubparserTopDownBase;
protected:
  std::shared_ptr<SymbolTableEntryImplBase> mRoutineId;
  PascalErrorHandler* mErrorHandler;
};

class PascalSubparserTopDownBase {
public:
  explicit PascalSubparserTopDownBase(PascalParserTopDown& pascal_parser);
  virtual ~PascalSubparserTopDownBase();
  std::shared_ptr<PascalToken> currentToken() const;
  std::shared_ptr<PascalToken> nextToken();
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  std::shared_ptr<SymbolTableStackImplBase> getSymbolTableStack();
//  std::shared_ptr<ICodeImplBase> getICode() const;
  std::shared_ptr<PascalScanner> scanner() const;
  int errorCount();
  PascalErrorHandler* errorHandler();
  PascalParserTopDown* currentParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
  static void setLineNumber(std::unique_ptr<ICodeNodeImplBase>& node,
                            const std::shared_ptr<PascalToken>& token);
  static const std::set<PascalTokenTypeImpl> statementStartSet;
  static const std::set<PascalTokenTypeImpl> statementFollowSet;
  static const std::set<PascalTokenTypeImpl> expressionStartSet;
  // constantStartSet is used by both CaseStatementParser and ConstantDefinitionsParser!
  static const std::set<PascalTokenTypeImpl> constantStartSet;
  static const std::set<PascalTokenTypeImpl> colonEqualsSet;
  static const std::set<PascalTokenTypeImpl> ofSet;
  static const std::set<PascalTokenTypeImpl> commaSet;
  static const std::set<PascalTokenTypeImpl> toDowntoSet;
  static const std::set<PascalTokenTypeImpl> doSet;
  static const std::set<PascalTokenTypeImpl> thenSet;
  static const std::set<PascalTokenTypeImpl> declarationStartSet;
  static const std::set<PascalTokenTypeImpl> typeStartSet;
  static const std::set<PascalTokenTypeImpl> varStartSet;
  static const std::set<PascalTokenTypeImpl> routineStartSet;
  static const std::set<PascalTokenTypeImpl> identifierSet;
//  static const std::set<PascalTokenTypeImpl> mConstantStartSet;
  static const std::set<PascalTokenTypeImpl> equalsSet;
  static const std::set<PascalTokenTypeImpl> nextStartSet;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> relOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> addOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> multOpsMap;
private:
  PascalParserTopDown& mPascalParser;
};

class PascalErrorHandler {
public:
  PascalErrorHandler();
  virtual ~PascalErrorHandler();
  void flag(const std::shared_ptr<PascalToken> &token, const PascalErrorCode errorCode, const PascalParserTopDown *parser);
  void abortTranslation(const PascalErrorCode errorCode, const PascalParserTopDown *parser);
  int errorCount() const;
private:
  static const int maxError = 25;
  static std::map<PascalErrorCode, std::string> errorMessageMap;
  int mErrorCount;
};

class PascalErrorToken: public PascalToken {
public:
  PascalErrorToken();
  PascalErrorToken(std::shared_ptr<Source> source, const PascalErrorCode errorCode,
                   const std::string& tokenText);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalWordToken: public PascalToken {
public:
  explicit PascalWordToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalStringToken: public PascalToken {
public:
  explicit PascalStringToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalSpecialSymbolToken: public PascalToken {
public:
  explicit PascalSpecialSymbolToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
};

class PascalNumberToken: public PascalToken {
public:
  explicit PascalNumberToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const;
  virtual void extract();
  virtual void extractNumber(std::string& text);
private:
  std::string unsignedIntegerDigits(std::string& text);
  long long computeIntegerValue(const std::string& digits);
  double computeFloatValue(const std::string &whole_digits, const std::string &fraction_digits,
                           const std::string &exponent_digits, char exponent_sign);
};

std::unique_ptr<PascalParserTopDown> createPascalParser(const std::string& language, const std::string& type,
  std::shared_ptr<Source> source);

#endif // PASCALFRONTEND_H
