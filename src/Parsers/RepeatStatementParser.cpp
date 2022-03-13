#include "RepeatStatementParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"

RepeatStatementParser::RepeatStatementParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNodeImplBase> RepeatStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the REPEAT
  token = nextToken();
  // create the LOOP and TEST node
  auto loop_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::LOOP);
  auto test_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::TEST);
  // parse the statement list terminated by the UNTIL token
  StatementParser statement_parser(currentParser());
  // the LOOP node is the parent of the statement subtrees
  statement_parser.parseList(token, loop_node, PascalTokenTypeImpl::UNTIL, PascalErrorCode::MISSING_UNTIL);
  token = currentToken();
  // parse the expression in the TEST node
  ExpressionParser expression_parser(currentParser());
  test_node->addChild(expression_parser.parse(token));
  loop_node->addChild(std::move(test_node));
  return loop_node;
}
