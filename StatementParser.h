#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class StatementParser : public PascalParserTopDown
{
public:
  explicit StatementParser(PascalParserTopDown *parent = nullptr);
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<Token> token);
protected:
  virtual void setLineNumber(ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> *node, std::shared_ptr<Token> token);
};

#endif // STATEMENTPARSER_H
