#include "ForStatementParser.h"
#include "AssignmentStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "TypeChecker.h"

ForStatementParser::ForStatementParser(const std::shared_ptr<PascalParserTopDown> &parent)
    : PascalSubparserTopDownBase(parent) {}

std::shared_ptr<ICodeNodeImplBase> ForStatementParser::parse(std::shared_ptr<PascalToken> token) {
  // consume the FOR
  token = nextToken();
  const auto target_token = token;
  // create the COMPOUND, LOOP and TEST nodes
  auto compound_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::COMPOUND));
  auto loop_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::LOOP));
  auto test_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::TEST));
  // parse the embedded initial assignment
  AssignmentStatementParser assignment_parser(currentParser());
  auto init_assign_node = assignment_parser.parse(token);
  const auto control_type = (init_assign_node != nullptr) ? init_assign_node->getTypeSpec() : Predefined::instance().undefinedType;
  // type check: the control variable's type must be integer or enumeration
  if (!TypeChecker::TypeChecking::isInteger(control_type) &&
      control_type->form() != TypeFormImpl::ENUMERATION) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
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
  auto rel_op_node = std::shared_ptr(createICodeNode(
      direction == PascalTokenTypeImpl::TO ? ICodeNodeTypeImpl::GT
                                           : ICodeNodeTypeImpl::LT));
  // copy the control VARIABLE node from the assignment node
  auto control_variable_node = *(init_assign_node->childrenBegin());
  rel_op_node->addChild(std::move(control_variable_node->copy()));
  // parse the termination expression
  ExpressionParser expression_parser(currentParser());
  auto expr_node = expression_parser.parse(token);
  const auto expr_type = (expr_node != nullptr) ? expr_node->getTypeSpec() : Predefined::instance().undefinedType;
  // type check: the termination expression type must be assignment compatible with the control variable's type
  if (!TypeChecker::TypeCompatibility::areAssignmentCompatible(control_type, expr_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  rel_op_node->addChild(std::move(expr_node));
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
  auto next_assign_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::ASSIGN));
  next_assign_node->addChild(std::move(control_variable_node->copy()));
  // create the arithmetic operator node
  // ADD for TO, or SUBTRACT for DOWNTO
  auto arithmetic_op_node = std::shared_ptr(createICodeNode(
      direction == PascalTokenTypeImpl::TO ? ICodeNodeTypeImpl::ADD
                                           : ICodeNodeTypeImpl::SUBTRACT));
  arithmetic_op_node->addChild(std::move(control_variable_node->copy()));
  auto one_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT));
  one_node->setAttribute<ICodeKeyTypeImpl::VALUE>(PascalInteger(1));
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
