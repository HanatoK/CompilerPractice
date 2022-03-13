#include "IfStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"

IfStatementParser::IfStatementParser(const std::shared_ptr<PascalParserTopDown> &parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNodeImplBase> IfStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the IF
  token = nextToken();
  // create the IF node
  auto if_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
      ICodeNodeTypeImpl::IF);
  // parse the expression and adopt it as the child node of the IF node
  ExpressionParser expression_parser(currentParser());
  if_node->addChild(expression_parser.parse(token));
  // synchronize to the THEN set
  token = synchronize(IfStatementParser::thenSet());
  if (token->type() == PascalTokenTypeImpl::THEN) {
    // consume it
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_THEN, currentParser());
  }
  // parse the then statement
  StatementParser statement_parser(currentParser());
  if_node->addChild(statement_parser.parse(token));
  token = currentToken();
  // look for an ELSE
  if (token->type() == PascalTokenTypeImpl::ELSE) {
    // consume ELSE
    token = nextToken();
    // parse the ELSE statement
    if_node->addChild(statement_parser.parse(token));
  }
  return if_node;
}

PascalSubparserTopDownBase::TokenTypeSet IfStatementParser::thenSet() {
  auto s = StatementParser::statementStartSet();
  s.insert(PascalTokenTypeImpl::THEN);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
