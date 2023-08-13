#ifndef COMPILERPRACTICE_CALLPARSER_H
#define COMPILERPRACTICE_CALLPARSER_H

#include "PascalFrontend.h"

class CallParser : public PascalSubparserTopDownBase
{
public:
  explicit CallParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~CallParser() override;
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
protected:
  std::shared_ptr<ICodeNodeImplBase> parseActualParameters(
      std::shared_ptr<PascalToken> token, const std::shared_ptr<SymbolTableEntryImplBase>& pfId,
      bool isDeclared, bool isReadReadln, bool isWriteWriteln);
private:
  void checkActualParameter(
      const std::shared_ptr<PascalToken>& token, const std::shared_ptr<SymbolTableEntryImplBase>& formalId,
      const std::shared_ptr<ICodeNodeImplBase>& node);
  std::shared_ptr<ICodeNodeImplBase> parseWriteSpec(std::shared_ptr<PascalToken> token);
};

class CallDeclaredParser : public CallParser
{
public:
  explicit CallDeclaredParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~CallDeclaredParser() override;
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

class CallStandardParser : public  CallParser
{
public:
  explicit CallStandardParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~CallStandardParser() override;
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
private:
  std::shared_ptr<ICodeNodeImplBase> parseReadReadln(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseWriteWriteln(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseEofEoln(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseAbsSqr(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseArctanCosExpLnSinSqrt(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parsePredSucc(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseChr(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseOdd(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseOrd(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  std::shared_ptr<ICodeNodeImplBase> parseRoundTrunc(
      const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
      const std::shared_ptr<SymbolTableEntryImplBase>& pfId);
  bool checkParmCount(const std::shared_ptr<PascalToken>& token, const std::shared_ptr<ICodeNodeImplBase>& node, size_t count);
};

#endif //COMPILERPRACTICE_CALLPARSER_H
