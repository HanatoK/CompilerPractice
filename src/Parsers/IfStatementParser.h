#ifndef IFSTATEMENTPARSER_H
#define IFSTATEMENTPARSER_H

#include "PascalFrontend.h"

class IfStatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet thenSet();
  explicit IfStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
};

#endif // IFSTATEMENTPARSER_H
