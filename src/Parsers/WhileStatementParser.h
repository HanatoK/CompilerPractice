#ifndef WHILESTATEMENTPARSER_H
#define WHILESTATEMENTPARSER_H

#include "PascalFrontend.h"

class WhileStatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet doSet();
  explicit WhileStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
};

#endif // WHILESTATEMENTPARSER_H
