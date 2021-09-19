#include "ForStatementParser.h"

ForStatementParser::ForStatementParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ForStatementParser::parse(std::shared_ptr<PascalToken> token)
{

}
