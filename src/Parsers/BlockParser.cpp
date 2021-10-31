#include "BlockParser.h"

BlockParser::BlockParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > BlockParser::parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id)
{
  // TODO
}
