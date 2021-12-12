#include "SimpleTypeParser.h"
#include "IntermediateImpl.h"

SimpleTypeParser::SimpleTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

SimpleTypeParser::~SimpleTypeParser()
{

}

std::unique_ptr<TypeSpecImplBase> SimpleTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{

}
