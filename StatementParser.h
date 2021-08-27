#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class StatementParser : public PascalSubparserTopDownBase
{
public:
  explicit StatementParser(PascalParserTopDown& parent);
  virtual ~StatementParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<Token> token);
  virtual void setLineNumber(std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node,
                             const std::shared_ptr<Token>& token);
  // TODO: figure out what does parseList do
  virtual void parseList(std::shared_ptr<Token> token,
                         std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &parent_node,
                         PascalTokenType terminator, PascalErrorCode error_code);
};

#endif // STATEMENTPARSER_H
