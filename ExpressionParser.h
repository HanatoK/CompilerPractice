#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "PascalFrontend.h"

#include <set>
#include <unordered_map>

class ExpressionParser : public PascalSubparserTopDownBase
{
public:
  explicit ExpressionParser(PascalParserTopDown& parent);
  virtual ~ExpressionParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
private:
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseSimpleExpression(std::shared_ptr<PascalToken> token);
  std::set<PascalTokenTypeImpl> mRelOps;
  std::set<PascalTokenTypeImpl> mAddOps;
  std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> mRelOpsMap;
  std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> mAddOpsMap;
};

#endif // EXPRESSIONPARSER_H
