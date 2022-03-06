#ifndef COMPOUNDSTATEMENTPARSER_H
#define COMPOUNDSTATEMENTPARSER_H

#include "PascalFrontend.h"

class CompoundStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit CompoundStatementParser(PascalParserTopDown& parent);
  virtual ~CompoundStatementParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
};

#endif // COMPOUNDSTATEMENTPARSER_H
