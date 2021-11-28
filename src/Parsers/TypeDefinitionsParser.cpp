#include "TypeDefinitionsParser.h"

TypeDefinitionsParser::TypeDefinitionsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

TypeDefinitionsParser::~TypeDefinitionsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::unique_ptr<ICodeNodeImplBase> TypeDefinitionsParser::parse(std::shared_ptr<PascalToken> token)
{
  // loop to parse a sequence of type definitions

}
