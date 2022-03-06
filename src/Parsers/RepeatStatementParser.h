#ifndef REPEATSTATEMENTPARSER_H
#define REPEATSTATEMENTPARSER_H

#include "PascalFrontend.h"

class RepeatStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit RepeatStatementParser(PascalParserTopDown& parent);
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
};

#endif // REPEATSTATEMENTPARSER_H
