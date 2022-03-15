#ifndef REPEATSTATEMENTPARSER_H
#define REPEATSTATEMENTPARSER_H

#include "PascalFrontend.h"

class RepeatStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit RepeatStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
};

#endif // REPEATSTATEMENTPARSER_H
