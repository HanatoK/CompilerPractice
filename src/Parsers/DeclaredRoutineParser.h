#ifndef COMPILERPRACTICE_DECLAREDROUTINEPARSER_H
#define COMPILERPRACTICE_DECLAREDROUTINEPARSER_H

#include "PascalFrontend.h"

class DeclaredRoutineParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet parameterSet();
  static TokenTypeSet leftParenSet();
  static TokenTypeSet rightParenSet();
  static TokenTypeSet parameterFollowSet();
  static TokenTypeSet commaSet();
  explicit DeclaredRoutineParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~DeclaredRoutineParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
private:
  int mDummyCounter;
private:
  std::shared_ptr<SymbolTableEntryImplBase> parseRoutineName(
      std::shared_ptr<PascalToken> token, const std::string& dummy_name);
  void parseHeader(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id);
  void parseFormalParameters(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id);
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> parseParmSublist(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id);
};


#endif //COMPILERPRACTICE_DECLAREDROUTINEPARSER_H
