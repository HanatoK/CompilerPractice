#include "CompoundStatementParser.h"
#include "StatementParser.h"
#include "IntermediateImpl.h"

CompoundStatementParser::CompoundStatementParser(PascalParserTopDown &parent)
  : PascalSubparserTopDownBase(parent) {}

CompoundStatementParser::~CompoundStatementParser()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > CompoundStatementParser::parse(std::shared_ptr<Token> token)
{
  // consume the BEGIN
  token = nextToken();
  // create the compound node
  auto compound_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::COMPOUND);
  // parse the statement list terminated by the END token
  StatementParser statement_parser(*currentParser());
  statement_parser.parseList(token, compound_node, PascalTokenType::END, PascalErrorCode::MISSING_END);
  return std::move(compound_node);
}
