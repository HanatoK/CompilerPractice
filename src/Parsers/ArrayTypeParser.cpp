#include "ArrayTypeParser.h"

ArrayTypeParser::ArrayTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

ArrayTypeParser::~ArrayTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> ArrayTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  std::shared_ptr<TypeSpecImplBase> array_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::ARRAY);
  // consume ARRAY
  nextToken();
  // synchronize at [
  token = synchronize(leftBracketSet);
  if (token->type() != PascalTokenTypeImpl::LEFT_BRACKET) {
    errorHandler()->flag(token, PascalErrorCode::MISSING_LEFT_BRACKET, currentParser());
  }
  // parse the list of index types
  auto element_type = parseIndexTypeList(token, array_type);
  // synchronize ]
  token = synchronize(rightBracketSet);
  if (token->type() != PascalTokenTypeImpl::RIGHT_BRACKET) {
    errorHandler()->flag(token, PascalErrorCode::MISSING_RIGHT_BRACKET, currentParser());
  } else {
    // consume ]
    token = nextToken();
  }
  // synchronize at OF
  if (token->type() == PascalTokenTypeImpl::OF) {
    // consume OF
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_OF, currentParser());
  }
  element_type->setAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE, parseElementType(token));
  return array_type;
}

std::shared_ptr<TypeSpecImplBase> ArrayTypeParser::parseIndexTypeList(
  std::shared_ptr<PascalToken>& token, std::shared_ptr<TypeSpecImplBase> array_type)
{

}
