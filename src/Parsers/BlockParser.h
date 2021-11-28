#ifndef BLOCKPARSER_H
#define BLOCKPARSER_H

#include "PascalFrontend.h"

class BlockParser : public PascalSubparserTopDownBase
{
public:
  explicit BlockParser(PascalParserTopDown &parent);
  virtual std::unique_ptr<ICodeNodeImplBase>
  parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id);
};

#endif // BLOCKPARSER_H
