#include "IfStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"

IfStatementParser::IfStatementParser(PascalParserTopDown &parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > IfStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the IF
  token = nextToken();
  // create the IF node
  auto if_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
      ICodeNodeTypeImpl::IF);
  // parse the expression and adopt it as the child node of the IF node
  ExpressionParser expression_parser(*currentParser());
  if_node->addChild(expression_parser.parse(token));
  // synchronize to the THEN set
  token = synchronize(mThenSet);
  if (token->type() == PascalTokenTypeImpl::THEN) {
    // consume it
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_THEN, currentParser());
  }
  // parse the then statement
  StatementParser statement_parser(*currentParser());
  if_node->addChild(statement_parser.parse(token));
  token = currentToken();
  // look for an ELSE
  if (token->type() == PascalTokenTypeImpl::ELSE) {
    // consume ELSE
    token = nextToken();
    // parse the ELSE statement
    if_node->addChild(statement_parser.parse(token));
  }
  return std::move(if_node);
}