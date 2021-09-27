#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "PascalFrontend.h"

#include <unordered_map>

class ExpressionParser : public PascalSubparserTopDownBase
{
public:
  explicit ExpressionParser(PascalParserTopDown& parent);
  virtual ~ExpressionParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
private:
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseFactor(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseTerm(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parseSimpleExpression(std::shared_ptr<PascalToken> token);
};

#endif // EXPRESSIONPARSER_H
