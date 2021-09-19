#include "IfStatementParser.h"


IfStatementParser::IfStatementParser(PascalParserTopDown &parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > IfStatementParser::parse(std::shared_ptr<PascalToken> token)
{

}
