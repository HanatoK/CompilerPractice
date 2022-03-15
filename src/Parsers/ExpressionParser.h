#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "PascalFrontend.h"

#include <unordered_map>

class ExpressionParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet expressionStartSet();
  explicit ExpressionParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~ExpressionParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
private:
  std::shared_ptr<ICodeNodeImplBase> parseFactor(std::shared_ptr<PascalToken> token);
  std::shared_ptr<ICodeNodeImplBase> parseTerm(std::shared_ptr<PascalToken> token);
  std::shared_ptr<ICodeNodeImplBase> parseSimpleExpression(std::shared_ptr<PascalToken> token);
};

#endif // EXPRESSIONPARSER_H
