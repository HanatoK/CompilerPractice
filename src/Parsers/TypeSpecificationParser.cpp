#include "TypeSpecificationParser.h"
#include "SimpleTypeParser.h"
#include "RecordTypeParser.h"
#include "ArrayTypeParser.h"

TypeSpecificationParser::TypeSpecificationParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::shared_ptr<TypeSpecImplBase> TypeSpecificationParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  token = synchronize(TypeSpecificationParser::typeStartSet());
  switch (token->type()) {
    case PascalTokenTypeImpl::ARRAY: {
      ArrayTypeParser type_parser(*currentParser());
      return type_parser.parseSpec(token);
    }
    case PascalTokenTypeImpl::RECORD: {
      RecordTypeParser type_parser(*currentParser());
      return type_parser.parseSpec(token);
    }
    default: {
      SimpleTypeParser type_parser(*currentParser());
      return type_parser.parseSpec(token);
    }
  }
}

PascalSubparserTopDownBase::TokenTypeSet TypeSpecificationParser::typeStartSet() {
  auto s = SimpleTypeParser::simpleTypeStartSet();
  s.insert({PascalTokenTypeImpl::ARRAY,
            PascalTokenTypeImpl::RECORD,
            PascalTokenTypeImpl::SEMICOLON});
  return s;
}
