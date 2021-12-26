#include "RecordTypeParser.h"

RecordTypeParser::RecordTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

RecordTypeParser::~RecordTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> RecordTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  // TODO
}
