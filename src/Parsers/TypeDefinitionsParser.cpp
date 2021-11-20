#include "TypeDefinitionsParser.h"

TypeDefinitionsParser::TypeDefinitionsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

TypeDefinitionsParser::~TypeDefinitionsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}
