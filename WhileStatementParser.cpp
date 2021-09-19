#include "WhileStatementParser.h"

WhileStatementParser::WhileStatementParser(PascalParserTopDown &parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > WhileStatementParser::parse(std::shared_ptr<PascalToken> token)
{

}
