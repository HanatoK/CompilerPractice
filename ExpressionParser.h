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
  static const std::set<PascalTokenTypeImpl> mExpressionStartSet;
private:
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseFactor(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseTerm(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseSimpleExpression(std::shared_ptr<PascalToken> token);
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> mRelOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> mAddOpsMap;
  static const std::unordered_map<PascalTokenTypeImpl, ICodeNodeTypeImpl> mMultOpsMap;
};

#endif // EXPRESSIONPARSER_H
