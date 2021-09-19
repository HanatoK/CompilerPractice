#include "RepeatStatementParser.h"

RepeatStatementParser::RepeatStatementParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > RepeatStatementParser::parse(std::shared_ptr<PascalToken> token)
{

}
