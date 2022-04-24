#include "RepeatStatementParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"
#include "TypeChecker.h"

RepeatStatementParser::RepeatStatementParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

std::shared_ptr<ICodeNodeImplBase> RepeatStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the REPEAT
  token = nextToken();
  // create the LOOP and TEST node
  auto loop_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::LOOP));
  auto test_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::TEST));
  // parse the statement list terminated by the UNTIL token
  StatementParser statement_parser(currentParser());
  // the LOOP node is the parent of the statement subtrees
  statement_parser.parseList(token, loop_node, PascalTokenTypeImpl::UNTIL, PascalErrorCode::MISSING_UNTIL);
  token = currentToken();
  // parse the expression in the TEST node
  ExpressionParser expression_parser(currentParser());
  auto expr_node = expression_parser.parse(token);
  const auto expr_type = (expr_node != nullptr) ? expr_node->getTypeSpec() : Predefined::instance().undefinedType;
  using namespace TypeChecker::TypeChecking;
  if (!isBoolean(expr_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  test_node->addChild(std::move(expr_node));
  loop_node->addChild(std::move(test_node));
  return loop_node;
}
