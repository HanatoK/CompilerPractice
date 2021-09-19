#include "CaseStatementParser.h"

CaseStatementParser::CaseStatementParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > CaseStatementParser::parse(std::shared_ptr<PascalToken> token)
{

}
