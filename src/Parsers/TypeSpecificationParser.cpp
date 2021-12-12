#include "TypeSpecificationParser.h"

TypeSpecificationParser::TypeSpecificationParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

std::unique_ptr<TypeSpecImplBase> TypeSpecificationParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  token = synchronize(typeStartSet);
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
