#ifndef FORSTATEMENTPARSER_H
#define FORSTATEMENTPARSER_H

#include "PascalFrontend.h"

class ForStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit ForStatementParser(PascalParserTopDown& parent);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // FORSTATEMENTPARSER_H
