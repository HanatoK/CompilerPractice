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
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> pfId,
      bool isDeclared, bool isReadReadln, bool isWriteWriteln);
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
  std::shared_ptr<ICodeNodeImplBase>
  parseReadReadln(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  // TODO: implement the following functions
  std::shared_ptr<ICodeNodeImplBase>
  parseWriteWriteln(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseEofEoln(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseAbsSqr(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseArctanCosExpLnSinSqrt(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parsePredSucc(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseChr(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseOdd(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseOrd(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
  std::shared_ptr<ICodeNodeImplBase>
  parseRoundTrunc(
      std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
      std::shared_ptr<SymbolTableEntryImplBase> pfId);
};

#endif //COMPILERPRACTICE_CALLPARSER_H
