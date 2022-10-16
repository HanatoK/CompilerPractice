#ifndef COMPOUNDSTATEMENTPARSER_H
#define COMPOUNDSTATEMENTPARSER_H

#include "PascalFrontend.h"

class CompoundStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit CompoundStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~CompoundStatementParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

#endif // COMPOUNDSTATEMENTPARSER_H
