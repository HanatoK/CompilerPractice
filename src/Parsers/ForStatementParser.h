#ifndef FORSTATEMENTPARSER_H
#define FORSTATEMENTPARSER_H

#include "PascalFrontend.h"

class ForStatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet toDownToSet();
  static TokenTypeSet doSet();
  explicit ForStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

#endif // FORSTATEMENTPARSER_H
