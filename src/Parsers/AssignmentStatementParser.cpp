#include "AssignmentStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "VariableParser.h"
#include "TypeChecker.h"

AssignmentStatementParser::AssignmentStatementParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

AssignmentStatementParser::~AssignmentStatementParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::shared_ptr<ICodeNodeImplBase> AssignmentStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // create the assign node
  auto assign_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::ASSIGN));
  // parse the target variable
  VariableParser variable_parser(currentParser());
  auto target_node = variable_parser.parse(token);
  const auto target_type = (target_node != nullptr) ? target_node->getTypeSpec() :
                                            Predefined::instance().undefinedType;
  // the ASSIGN node adopts the variable node as its first child
  assign_node->addChild(target_node);
  // synchronize on the := token
  token = synchronize(AssignmentStatementParser::colonEqualsSet());
  // and look for the := token
  if (token->type() == PascalTokenTypeImpl::COLON_EQUALS) {
    // consume :=
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_COLON_EQUALS, currentParser());
  }
  // parse the expression
  ExpressionParser expression_parser(currentParser());
  auto expression_node = expression_parser.parse(token);
  // type check: assignment compatible?
  const auto expression_type = (expression_node != nullptr) ?
                               expression_node->getTypeSpec() : Predefined::instance().undefinedType;
  using namespace TypeChecker::TypeCompatibility;
  if (!areAssignmentCompatible(target_type, expression_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  assign_node->addChild(std::move(expression_node));
  assign_node->setTypeSpec(target_type);
  return assign_node;
}

PascalSubparserTopDownBase::TokenTypeSet AssignmentStatementParser::colonEqualsSet() {
  auto s = ExpressionParser::expressionStartSet();
  s.insert(PascalTokenTypeImpl::COLON_EQUALS);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
