#include "EnumerationTypeParser.h"
#include "DeclarationsParser.h"

EnumerationTypeParser::EnumerationTypeParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

EnumerationTypeParser::~EnumerationTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> EnumerationTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  auto enumeration_type = std::shared_ptr(createType(TypeFormImpl::ENUMERATION));
  PascalInteger value = -1;
  std::vector<std::weak_ptr<SymbolTableEntryImplBase>> constants;
  // consume the opening (
  token = nextToken();
  do {
    token = synchronize(EnumerationTypeParser::enumConstantStartSet());
    parseEnumerationIdentifier(token, ++value, enumeration_type, constants);
    token = currentToken();
    const auto token_type = token->type();
    if (token_type == PascalTokenTypeImpl::COMMA) {
      // consume the ,
      token = nextToken();
      if (enumDefinitionFollowSet().contains(token->type())) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
      }
    } else if (enumConstantStartSet().contains(token_type)) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_COMMA, currentParser());
    }
  } while (!enumDefinitionFollowSet().contains(token->type()));
  if (token->type() == PascalTokenTypeImpl::RIGHT_PAREN) {
    // consume )
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_RIGHT_PAREN, currentParser());
  }
  enumeration_type->setAttribute(TypeKeyImpl::ENUMERATION_CONSTANTS, constants);
  return enumeration_type;
}

void EnumerationTypeParser::parseEnumerationIdentifier(
  std::shared_ptr<PascalToken>& token, PascalInteger value, std::shared_ptr<TypeSpecImplBase> enumeration_type,
  std::vector<std::weak_ptr<SymbolTableEntryImplBase> >& constants)
{
  const auto token_type = token->type();
  if (token_type == PascalTokenTypeImpl::IDENTIFIER) {
    const auto name = boost::algorithm::to_lower_copy(token->text());
    auto constant_id = getSymbolTableStack()->lookupLocal(name);
    if (constant_id != nullptr) {
      errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_REDEFINED, currentParser());
    } else {
      constant_id = getSymbolTableStack()->enterLocal(token->text());
      constant_id->setDefinition(DefinitionImpl::ENUMERATION_CONSTANT);
      constant_id->setTypeSpec(enumeration_type);
      constant_id->setAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE, value);
      constant_id->appendLineNumber(token->lineNum());
      constants.push_back(constant_id);
    }
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
  }
}

PascalSubparserTopDownBase::TokenTypeSet EnumerationTypeParser::enumConstantStartSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::COMMA
  };
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet EnumerationTypeParser::enumDefinitionFollowSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::RIGHT_PAREN,
    PascalTokenTypeImpl::SEMICOLON
  };
  s.merge(DeclarationsParser::varStartSet());
  return s;
}
