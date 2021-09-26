#ifndef IFSTATEMENTPARSER_H
#define IFSTATEMENTPARSER_H

#include "PascalFrontend.h"

class IfStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit IfStatementParser(PascalParserTopDown& parent);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
  static const std::set<PascalTokenTypeImpl> mThenSet;
};

#endif // IFSTATEMENTPARSER_H
