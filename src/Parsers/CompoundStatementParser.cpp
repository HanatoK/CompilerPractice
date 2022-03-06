#include "CompoundStatementParser.h"
#include "StatementParser.h"
#include "IntermediateImpl.h"

CompoundStatementParser::CompoundStatementParser(PascalParserTopDown &parent)
  : PascalSubparserTopDownBase(parent) {}

CompoundStatementParser::~CompoundStatementParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::unique_ptr<ICodeNodeImplBase> CompoundStatementParser::parse(std::shared_ptr<PascalToken> token)
{
  // consume the BEGIN
  token = nextToken();
  // create the compound node
  auto compound_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::COMPOUND);
  // parse the statement list terminated by the END token
  StatementParser statement_parser(*currentParser());
  statement_parser.parseList(token, compound_node, PascalTokenTypeImpl::END, PascalErrorCode::MISSING_END);
  return compound_node;
}
