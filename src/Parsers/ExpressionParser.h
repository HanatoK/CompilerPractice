#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "PascalFrontend.h"

#include <unordered_map>

class ExpressionParser : public PascalSubparserTopDownBase
{
public:
  explicit ExpressionParser(PascalParserTopDown& parent);
  virtual ~ExpressionParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
private:
  std::unique_ptr<ICodeNodeImplBase> parseFactor(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNodeImplBase> parseTerm(std::shared_ptr<PascalToken> token);
  std::unique_ptr<ICodeNodeImplBase> parseSimpleExpression(std::shared_ptr<PascalToken> token);
};

#endif // EXPRESSIONPARSER_H
