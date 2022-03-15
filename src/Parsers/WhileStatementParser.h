#ifndef WHILESTATEMENTPARSER_H
#define WHILESTATEMENTPARSER_H

#include "PascalFrontend.h"

class WhileStatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet doSet();
  explicit WhileStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
};

#endif // WHILESTATEMENTPARSER_H
