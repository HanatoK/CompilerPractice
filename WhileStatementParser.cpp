#include "WhileStatementParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"

std::set<PascalTokenTypeImpl> initDoSet() {
  std::set<PascalTokenTypeImpl> s = StatementParser::mStatementStartSet;
  s.insert(PascalTokenTypeImpl::DO);
  const auto& follow_set = PascalSubparserTopDownBase::mStatementFollowSet;
  for (auto it = follow_set.begin(); it != follow_set.end(); ++it) {
    s.insert(*it);
  }
  return s;
}

decltype(WhileStatementParser::mDoSet) WhileStatementParser::mDoSet = initDoSet();

WhileStatementParser::WhileStatementParser(PascalParserTopDown &parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > WhileStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the WHILE
  token = nextToken();
  // create LOOP, TEST and NOT nodes
  auto loop_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::LOOP);
  auto break_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::TEST);
  // a WHILE loop exits when the test expression is false
  // this is adverse to the REPEAT node, so if NOT is used in REPEAT then it should not be used here
  auto not_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::NOT);
  // parse the expression as a child of the NOT node
  ExpressionParser expression_parser(*currentParser());
  // construct the parse tree
  not_node->addChild(expression_parser.parse(token));
  break_node->addChild(std::move(not_node));
  loop_node->addChild(std::move(break_node));
  token = synchronize(mDoSet);
  if (token->type() == PascalTokenTypeImpl::DO) {
    // consume the DO
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_DO, currentParser());
  }
  // parse the statements
  StatementParser statement_parser(*currentParser());
  loop_node->addChild(statement_parser.parse(token));
  return std::move(loop_node);
}
