#include "WhileStatementParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"
#include "TypeChecker.h"

WhileStatementParser::WhileStatementParser(const std::shared_ptr<PascalParserTopDown> &parent): PascalSubparserTopDownBase(parent)
{

}

std::shared_ptr<ICodeNodeImplBase> WhileStatementParser::parse(
    std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id)
{
  // consume the WHILE
  token = nextToken();
  // create LOOP, TEST and NOT nodes
  auto loop_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::LOOP));
  auto break_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::TEST));
  // a WHILE loop exits when the test expression is false
  // this is adverse to the REPEAT node, so if NOT is used in REPEAT then it should not be used here
  auto not_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::NOT));
  // parse the expression as a child of the NOT node
  ExpressionParser expression_parser(currentParser());
  auto expr_node = expression_parser.parse(token, parent_id);
  // type check: the test expression must be boolean
  const auto expr_type = (expr_node != nullptr) ? expr_node->getTypeSpec() : Predefined::instance().undefinedType;
  if (!TypeChecker::TypeChecking::isBoolean(expr_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  // construct the parse tree
  not_node->addChild(std::move(expr_node));
  break_node->addChild(std::move(not_node));
  loop_node->addChild(std::move(break_node));
  token = synchronize(WhileStatementParser::doSet());
  if (token->type() == PascalTokenTypeImpl::DO) {
    // consume the DO
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_DO, currentParser());
  }
  // parse the statements
  StatementParser statement_parser(currentParser());
  loop_node->addChild(statement_parser.parse(token, parent_id));
  return loop_node;
}

PascalSubparserTopDownBase::TokenTypeSet WhileStatementParser::doSet() {
  auto s = StatementParser::statementStartSet();
  s.insert(PascalTokenTypeImpl::DO);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
