#ifndef CASESTATEMENTPARSER_H
#define CASESTATEMENTPARSER_H

#include "PascalFrontend.h"

class CaseStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit CaseStatementParser(PascalParserTopDown& parent);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // CASESTATEMENTPARSER_H
