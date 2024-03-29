#ifndef REPEATSTATEMENTPARSER_H
#define REPEATSTATEMENTPARSER_H

#include "PascalFrontend.h"

class RepeatStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit RepeatStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

#endif // REPEATSTATEMENTPARSER_H
