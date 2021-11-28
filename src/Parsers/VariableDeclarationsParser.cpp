#include "VariableDeclarationsParser.h"

VariableDeclarationsParser::VariableDeclarationsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

VariableDeclarationsParser::~VariableDeclarationsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::unique_ptr<ICodeNodeImplBase> VariableDeclarationsParser::parse(std::shared_ptr<PascalToken> token)
{

}
