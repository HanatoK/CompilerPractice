#include "EnumerationTypeParser.h"

EnumerationTypeParser::EnumerationTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

EnumerationTypeParser::~EnumerationTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> EnumerationTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  std::shared_ptr<TypeSpecImplBase> enumeration_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::ENUMERATION);
  int value = -1;
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> constants;
  // consume the opening (
  token = nextToken();
  do {
    token = synchronize(enumConstantStartSet);
    parseEnumerationIdentifier(token, ++value, enumeration_type, constants);
    token = nextToken();
    const auto token_type = token->type();
    if (token_type == PascalTokenTypeImpl::COMMA) {
      // consume the ,
      token = nextToken();
      if (enumDefinitionFollowSet.contains(token->type())) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
      }
    } else if (enumConstantStartSet.contains(token_type)) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_COMMA, currentParser());
    }
  } while (!enumConstantStartSet.contains(token->type()));
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
  std::shared_ptr<PascalToken>& token, int value, std::shared_ptr<TypeSpecImplBase> enumeration_type,
  std::vector<std::shared_ptr<SymbolTableEntryImplBase> >& constants)
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
