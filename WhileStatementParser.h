#ifndef WHILESTATEMENTPARSER_H
#define WHILESTATEMENTPARSER_H

#include "PascalFrontend.h"

class WhileStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit WhileStatementParser(PascalParserTopDown& parent);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
  static const std::set<PascalTokenTypeImpl> mDoSet;
};

std::set<PascalTokenTypeImpl> initDoSet();

#endif // WHILESTATEMENTPARSER_H
