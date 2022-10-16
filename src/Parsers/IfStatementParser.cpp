#include "IfStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "TypeChecker.h"

IfStatementParser::IfStatementParser(const std::shared_ptr<PascalParserTopDown> &parent): PascalSubparserTopDownBase(parent)
{

}

std::shared_ptr<ICodeNodeImplBase> IfStatementParser::parse(
    std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id)
{
  // consume the IF
  token = nextToken();
  // create the IF node
  auto if_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::IF));
  // parse the expression and adopt it as the child node of the IF node
  ExpressionParser expression_parser(currentParser());
  auto expr_node = expression_parser.parse(token, parent_id);
  const auto expr_type = (expr_node != nullptr) ? expr_node->getTypeSpec() : Predefined::instance().undefinedType;
  if (!TypeChecker::TypeChecking::isBoolean(expr_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  if_node->addChild(std::move(expr_node));
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
  if_node->addChild(statement_parser.parse(token, parent_id));
  token = currentToken();
  // look for an ELSE
  if (token->type() == PascalTokenTypeImpl::ELSE) {
    // consume ELSE
    token = nextToken();
    // parse the ELSE statement
    if_node->addChild(statement_parser.parse(token, parent_id));
  }
  return if_node;
}

PascalSubparserTopDownBase::TokenTypeSet IfStatementParser::thenSet() {
  auto s = StatementParser::statementStartSet();
  s.insert(PascalTokenTypeImpl::THEN);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
