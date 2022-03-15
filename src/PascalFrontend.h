#ifndef PASCALFRONTEND_H
#define PASCALFRONTEND_H

#include "Frontend.h"
#include "Intermediate.h"
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
  virtual std::shared_ptr<PascalToken> extractToken() override;
private:
  void skipWhiteSpace();
};

class PascalParserTopDown:
  public Parser<SymbolTableKeyTypeImpl, DefinitionImpl,
                TypeFormImpl, TypeKeyImpl, ICodeNodeTypeImpl,
                ICodeKeyTypeImpl, PascalTokenTypeImpl, PascalScanner>,
  public std::enable_shared_from_this<PascalParserTopDown> {
public:
  explicit PascalParserTopDown(std::shared_ptr<PascalScanner> scanner);
  PascalParserTopDown(const PascalParserTopDown&) = delete;
  virtual ~PascalParserTopDown();
  virtual void parse() override;
  virtual int errorCount() const override;
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  boost::signals2::signal<void(const int, const int, const PascalTokenTypeImpl, const std::string&, std::any)> pascalTokenMessage;
  boost::signals2::signal<void(const int, const int, const float)> parserSummary;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&, const std::any&)> tokenMessage;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&)> syntaxErrorMessage;
  friend class PascalSubparserTopDownBase;
protected:
  std::shared_ptr<SymbolTableEntryImplBase> mRoutineId;
  std::shared_ptr<PascalErrorHandler> mErrorHandler;
};

class PascalSubparserTopDownBase {
public:
  typedef std::set<PascalTokenTypeImpl> TokenTypeSet;
  explicit PascalSubparserTopDownBase(const std::shared_ptr<PascalParserTopDown>& pascal_parser);
  virtual ~PascalSubparserTopDownBase();
  std::shared_ptr<PascalToken> currentToken() const;
  std::shared_ptr<PascalToken> nextToken();
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  std::shared_ptr<SymbolTableStackImplBase> getSymbolTableStack();
  std::shared_ptr<PascalScanner> scanner() const;
  int errorCount();
  std::shared_ptr<PascalErrorHandler> errorHandler();
  std::shared_ptr<PascalParserTopDown> currentParser();
  // TODO: what exactly does this function return??
  virtual std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
  static void setLineNumber(std::shared_ptr<ICodeNodeImplBase>& node,
                            const std::shared_ptr<PascalToken>& token);
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> relOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> addOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> multOpsMap;
private:
  std::shared_ptr<PascalParserTopDown> mPascalParser;
};

class PascalErrorHandler {
public:
  PascalErrorHandler();
  virtual ~PascalErrorHandler();
  void flag(const std::shared_ptr<PascalToken> &token, const PascalErrorCode errorCode,
            const std::shared_ptr<PascalParserTopDown>& parser);
  static void abortTranslation(const PascalErrorCode errorCode, const std::shared_ptr<PascalParserTopDown>& parser);
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
  virtual unique_ptr<PascalToken> clone() const override;
  virtual void extract() override;
};

class PascalWordToken: public PascalToken {
public:
  explicit PascalWordToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const override;
  virtual void extract() override;
};

class PascalStringToken: public PascalToken {
public:
  explicit PascalStringToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const override;
  virtual void extract() override;
};

class PascalSpecialSymbolToken: public PascalToken {
public:
  explicit PascalSpecialSymbolToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const override;
  virtual void extract() override;
};

class PascalNumberToken: public PascalToken {
public:
  explicit PascalNumberToken(std::shared_ptr<Source> source);
  virtual unique_ptr<PascalToken> clone() const override;
  virtual void extract() override;
  virtual void extractNumber(std::string& text);
private:
  std::string unsignedIntegerDigits(std::string& text);
  PascalInteger computeIntegerValue(const std::string& digits);
  PascalFloat computeFloatValue(
      const std::string &whole_digits, const std::string &fraction_digits,
      const std::string &exponent_digits, char exponent_sign);
};

std::unique_ptr<PascalParserTopDown> createPascalParser(const std::string& language, const std::string& type,
  const std::shared_ptr<Source>& source);

#endif // PASCALFRONTEND_H
