#include "ForStatementParser.h"
#include "AssignmentStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"

ForStatementParser::ForStatementParser(const std::shared_ptr<PascalParserTopDown> &parent)
    : PascalSubparserTopDownBase(parent) {}

std::shared_ptr<ICodeNodeImplBase> ForStatementParser::parse(std::shared_ptr<PascalToken> token) {
  // cosume the FOR
  token = nextToken();
  const auto target_token = token;
  // create the COMPOUND, LOOP and TEST nodes
  std::shared_ptr<ICodeNodeImplBase> compound_node = createICodeNode(ICodeNodeTypeImpl::COMPOUND);
  std::shared_ptr<ICodeNodeImplBase> loop_node = createICodeNode(ICodeNodeTypeImpl::LOOP);
  std::shared_ptr<ICodeNodeImplBase> test_node = createICodeNode(ICodeNodeTypeImpl::TEST);
  // parse the embedded initial assignment
  AssignmentStatementParser assignment_parser(currentParser());
  auto init_assign_node = assignment_parser.parse(token);
  // set the current line number attribute
  setLineNumber(init_assign_node, target_token);
  // synchronize at the TO or DOWNTO
  token = synchronize(ForStatementParser::toDownToSet());
  auto direction = token->type();
  if (direction == PascalTokenTypeImpl::TO ||
      direction == PascalTokenTypeImpl::DOWNTO) {
    // consume the TO or DOWNTO
    token = nextToken();
  } else {
    direction = PascalTokenTypeImpl::TO;
    errorHandler()->flag(token, PascalErrorCode::MISSING_TO_DOWNTO,
                         currentParser());
  }
  // create a relational operator node
  std::shared_ptr<ICodeNodeImplBase> rel_op_node = createICodeNode(
      direction == PascalTokenTypeImpl::TO ? ICodeNodeTypeImpl::GT
                                           : ICodeNodeTypeImpl::LT);
  // copy the control VARIABLE node from the assignment node
  auto control_variable_node = *(init_assign_node->childrenBegin());
  rel_op_node->addChild(std::move(control_variable_node->copy()));
  // parse the termination expression
  ExpressionParser expression_parser(currentParser());
  rel_op_node->addChild(expression_parser.parse(token));
  test_node->addChild(std::move(rel_op_node));
  loop_node->addChild(std::move(test_node));
  // synchronize to the DO set
  token = synchronize(ForStatementParser::doSet());
  if (token->type() == PascalTokenTypeImpl::DO) {
    // consume the DO token
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_DO, currentParser());
  }
  // parse the nested statement
  StatementParser statement_parser(currentParser());
  loop_node->addChild(statement_parser.parse(token));
  // create an assignment with a copy of the control variable to advance the value of it
  std::shared_ptr<ICodeNodeImplBase> next_assign_node = createICodeNode(ICodeNodeTypeImpl::ASSIGN);
  next_assign_node->addChild(std::move(control_variable_node->copy()));
  // create the arithmetic operator node
  // ADD for TO, or SUBTRACT for DOWNTO
  std::shared_ptr<ICodeNodeImplBase> arithmetic_op_node = createICodeNode(
      direction == PascalTokenTypeImpl::TO ? ICodeNodeTypeImpl::ADD
                                           : ICodeNodeTypeImpl::SUBTRACT);
  arithmetic_op_node->addChild(std::move(control_variable_node->copy()));
  std::shared_ptr<ICodeNodeImplBase> one_node = createICodeNode(
      ICodeNodeTypeImpl::INTEGER_CONSTANT);
  one_node->setAttribute(ICodeKeyTypeImpl::VALUE, PascalInteger(1));
  arithmetic_op_node->addChild(std::move(one_node));
  // the next ASSIGN node adopts the arithmetic operator node as its second child,
  next_assign_node->addChild(std::move(arithmetic_op_node));
  setLineNumber(next_assign_node, target_token);
  loop_node->addChild(std::move(next_assign_node));
  compound_node->addChild(std::move(init_assign_node));
  compound_node->addChild(std::move(loop_node));
  return compound_node;
}

PascalSubparserTopDownBase::TokenTypeSet ForStatementParser::toDownToSet() {
  auto s = ExpressionParser::expressionStartSet();
  s.insert({PascalTokenTypeImpl::TO,
            PascalTokenTypeImpl::DOWNTO});
  s.merge(StatementParser::statementFollowSet());
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ForStatementParser::doSet() {
  auto s = StatementParser::statementStartSet();
  s.insert(PascalTokenTypeImpl::DO);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
