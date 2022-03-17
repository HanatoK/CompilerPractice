#include "AssignmentStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"

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
  std::shared_ptr<ICodeNodeImplBase> assign_node = createICodeNode(ICodeNodeTypeImpl::ASSIGN);
  // lookup the target identifier in the symbol table stack
  const std::string target_name = boost::algorithm::to_lower_copy(token->text());
  auto target_id = getSymbolTableStack()->lookup(target_name);
  // enter the identifier into the table if not found
  if (target_id == nullptr) {
    target_id = getSymbolTableStack()->enterLocal(target_name);
  }
  target_id->appendLineNumber(token->lineNum());
  // create the variable node and set its name attribute
  auto variable_node = createICodeNode(ICodeNodeTypeImpl::VARIABLE);
  variable_node->setAttribute(ICodeKeyTypeImpl::ID, target_id);
  // the assign node adopts the variable node as its child
  assign_node->addChild(std::move(variable_node));
  // consume the identifier token
  nextToken();
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
  assign_node->addChild(expression_parser.parse(token));
  return assign_node;
}

PascalSubparserTopDownBase::TokenTypeSet AssignmentStatementParser::colonEqualsSet() {
  auto s = ExpressionParser::expressionStartSet();
  s.insert(PascalTokenTypeImpl::COLON_EQUALS);
  s.merge(StatementParser::statementFollowSet());
  return s;
}
