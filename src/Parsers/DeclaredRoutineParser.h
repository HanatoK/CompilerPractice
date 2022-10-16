#ifndef COMPILERPRACTICE_DECLAREDROUTINEPARSER_H
#define COMPILERPRACTICE_DECLAREDROUTINEPARSER_H

#include "PascalFrontend.h"

class DeclaredRoutineParser : public PascalSubparserTopDownBase
{
public:
  explicit DeclaredRoutineParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~DeclaredRoutineParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
private:
  int mDummyCounter;
private:
  std::shared_ptr<SymbolTableEntryImplBase> parseRoutineName(
      std::shared_ptr<PascalToken> token, const std::string& dummy_name);
};


#endif //COMPILERPRACTICE_DECLAREDROUTINEPARSER_H
