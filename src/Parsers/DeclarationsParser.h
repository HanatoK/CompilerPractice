#ifndef DECLARATIONSPARSER_H
#define DECLARATIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class DeclarationsParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet declarationStartSet();
  static TokenTypeSet typeStartSet();
  static TokenTypeSet varStartSet();
  static TokenTypeSet routineStartSet();
  explicit DeclarationsParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~DeclarationsParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

#endif // DECLARATIONSPARSER_H
