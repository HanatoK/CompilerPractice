#include "SimpleTypeParser.h"
#include "IntermediateImpl.h"
#include "SubrangeTypeParser.h"
#include "EnumerationTypeParser.h"
#include "ConstantDefinitionsParser.h"

SimpleTypeParser::SimpleTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

SimpleTypeParser::~SimpleTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> SimpleTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  token = synchronize(SimpleTypeParser::simpleTypeStartSet());
  switch (token->type()) {
    case PascalTokenTypeImpl::IDENTIFIER: {
      std::string name = boost::algorithm::to_lower_copy(token->text());
      auto id = getSymbolTableStack()->lookup(name);
      if (id != nullptr) {
        auto definition = id->getDefinition();
        // it's either a type identifier or the start of a subrange
        if (definition == DefinitionImpl::TYPE) {
          id->appendLineNumber(token->lineNum());
          // consume the identifier
          token = nextToken();
          return id->getTypeSpec();
        } else if ((definition != DefinitionImpl::CONSTANT) &&
                   (definition != DefinitionImpl::ENUMERATION_CONSTANT)) {
          errorHandler()->flag(token, PascalErrorCode::NOT_TYPE_IDENTIFIER, currentParser());
          token = nextToken();
          return nullptr;
        } else {
          SubrangeTypeParser parser(*currentParser());
          return parser.parseSpec(token);
        }
      } else {
        errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
        token = nextToken();
        return nullptr;
      }
    }
    case PascalTokenTypeImpl::LEFT_PAREN: {
      EnumerationTypeParser parser(*currentParser());
      return parser.parseSpec(token);
    }
    case PascalTokenTypeImpl::COMMA:
    case PascalTokenTypeImpl::SEMICOLON: {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
      return nullptr;
    }
    default: {
      SubrangeTypeParser parser(*currentParser());
      return parser.parseSpec(token);
    }
  }
}

PascalSubparserTopDownBase::TokenTypeSet SimpleTypeParser::simpleTypeStartSet() {
  auto s = ConstantDefinitionsParser::constantStartSet();
  s.insert({PascalTokenTypeImpl::LEFT_PAREN,
              PascalTokenTypeImpl::COMMA,
              PascalTokenTypeImpl::SEMICOLON});
  return s;
}
