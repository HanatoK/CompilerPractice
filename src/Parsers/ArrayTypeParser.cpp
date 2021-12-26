#include "ArrayTypeParser.h"

ArrayTypeParser::ArrayTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

ArrayTypeParser::~ArrayTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> ArrayTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  // TODO
}
