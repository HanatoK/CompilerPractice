#include "ConstantDefinitionsParser.h"

ConstantDefinitionsParser::ConstantDefinitionsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

ConstantDefinitionsParser::~ConstantDefinitionsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}
