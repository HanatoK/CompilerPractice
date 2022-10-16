#ifndef BLOCKPARSER_H
#define BLOCKPARSER_H

#include "PascalFrontend.h"

class BlockParser : public PascalSubparserTopDownBase
{
public:
  explicit BlockParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::shared_ptr<ICodeNodeImplBase>
  parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
};

#endif // BLOCKPARSER_H
