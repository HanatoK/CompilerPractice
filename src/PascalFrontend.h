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
  ~PascalScanner() override;
  std::shared_ptr<PascalToken> extractToken() override;
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
  ~PascalParserTopDown() override;
  void parse() override;
  int errorCount() const override;
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  boost::signals2::signal<void(const int, const int, const PascalTokenTypeImpl, const std::string&, std::any)> pascalTokenMessage;
  boost::signals2::signal<void(const int, const int, const float)> parserSummary;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&, const std::any&)> tokenMessage;
  boost::signals2::signal<void(const int, const int, const std::string&, const std::string&)> syntaxErrorMessage;
  friend class PascalSubparserTopDownBase;
protected:
  std::shared_ptr<SymbolTableEntryImplBase> mRoutineId;
  std::shared_ptr<PascalErrorHandler> mErrorHandler;
  std::shared_ptr<ICodeNodeImplBase> mRootNode;
};

class PascalSubparserTopDownBase {
public:
  using TokenTypeSet = std::set<PascalTokenTypeImpl>;
  explicit PascalSubparserTopDownBase(const std::shared_ptr<PascalParserTopDown>& pascal_parser);
  virtual ~PascalSubparserTopDownBase();
  [[nodiscard]] std::shared_ptr<PascalToken> currentToken() const;
  std::shared_ptr<PascalToken> nextToken();
  std::shared_ptr<PascalToken> synchronize(const std::set<PascalTokenTypeImpl>& sync_set);
  std::shared_ptr<SymbolTableStackImplBase> getSymbolTableStack();
  [[nodiscard]] std::shared_ptr<PascalScanner> scanner() const;
  [[nodiscard]] int errorCount();
  std::shared_ptr<PascalErrorHandler> errorHandler();
  std::shared_ptr<PascalParserTopDown> currentParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
    std::shared_ptr<PascalToken> token,
    std::shared_ptr<SymbolTableEntryImplBase> parent_id);
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
  void flag(const std::shared_ptr<PascalToken> &token, PascalErrorCode errorCode,
            const std::shared_ptr<PascalParserTopDown>& parser);
  static void abortTranslation(PascalErrorCode errorCode, const std::shared_ptr<PascalParserTopDown>& parser);
  [[nodiscard]] int errorCount() const;
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
  [[nodiscard]] unique_ptr<PascalToken> clone() const override;
  void extract() override;
};

class PascalWordToken: public PascalToken {
public:
  explicit PascalWordToken(std::shared_ptr<Source> source);
  [[nodiscard]] unique_ptr<PascalToken> clone() const override;
  void extract() override;
};

class PascalStringToken: public PascalToken {
public:
  explicit PascalStringToken(std::shared_ptr<Source> source);
  [[nodiscard]] unique_ptr<PascalToken> clone() const override;
  void extract() override;
};

class PascalSpecialSymbolToken: public PascalToken {
public:
  explicit PascalSpecialSymbolToken(std::shared_ptr<Source> source);
  [[nodiscard]] unique_ptr<PascalToken> clone() const override;
  void extract() override;
};

class PascalNumberToken: public PascalToken {
public:
  explicit PascalNumberToken(std::shared_ptr<Source> source);
  [[nodiscard]] unique_ptr<PascalToken> clone() const override;
  void extract() override;
  void extractNumber(std::string& text);
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
