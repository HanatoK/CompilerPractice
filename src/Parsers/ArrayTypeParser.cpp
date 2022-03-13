#include "ArrayTypeParser.h"
#include "SimpleTypeParser.h"
#include "TypeSpecificationParser.h"

ArrayTypeParser::ArrayTypeParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

ArrayTypeParser::~ArrayTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> ArrayTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  // TODO: Fix nested array parser!
  std::shared_ptr<TypeSpecImplBase> array_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::ARRAY);
  // consume ARRAY
  nextToken();
  // synchronize at [
  token = synchronize(ArrayTypeParser::leftBracketSet());
  if (token->type() != PascalTokenTypeImpl::LEFT_BRACKET) {
    errorHandler()->flag(token, PascalErrorCode::MISSING_LEFT_BRACKET, currentParser());
  }
  // parse the list of index types
  auto element_type = parseIndexTypeList(token, array_type);
  // synchronize ]
  token = synchronize(ArrayTypeParser::rightBracketSet());
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
  auto element_type = array_type;
  bool another_index = false;
  // consume [
  token = nextToken();
  // parse the list of index type specifications
  do {
    another_index = false;
    // parse the index type
    token = synchronize(ArrayTypeParser::indexStartSet());
    parseIndexType(token, element_type);
    // synchronize at the , token
    token = synchronize(ArrayTypeParser::indexFollowSet());
    const auto token_type = token->type();
    if (token_type != PascalTokenTypeImpl::COMMA &&
        token_type != PascalTokenTypeImpl::RIGHT_BRACKET) {
      if (ArrayTypeParser::indexStartSet().contains(token_type)) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_COMMA, currentParser());
        another_index = true;
      }
    } else if (token_type == PascalTokenTypeImpl::COMMA) {
      std::shared_ptr<TypeSpecImplBase> new_element_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::ARRAY);
      element_type->setAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE, new_element_type);
      // is this work?
      element_type = std::move(new_element_type);
      token = nextToken();
      another_index = true;
    }
  } while (another_index);
  return element_type;
}

void ArrayTypeParser::parseIndexType(std::shared_ptr<PascalToken>& token,
                                     const std::shared_ptr<TypeSpecImplBase>& array_type)
{
  SimpleTypeParser simple_type_parser(currentParser());
  const auto index_type = simple_type_parser.parseSpec(token);
  array_type->setAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE, index_type);
  if (index_type == nullptr) {
    return;
  }
  const auto form = index_type->form();
  PascalInteger count = 0;
  // check the index type and set the element count
  if (form == TypeFormImpl::SUBRANGE) {
    const auto min_value = index_type->getAttribute(TypeKeyImpl::SUBRANGE_MIN_VALUE);
    const auto max_value = index_type->getAttribute(TypeKeyImpl::SUBRANGE_MAX_VALUE);
    if (min_value.has_value() && max_value.has_value()) {
      count = std::any_cast<PascalInteger>(max_value) - std::any_cast<PascalInteger>(min_value) + 1;
    }
  } else if (form == TypeFormImpl::ENUMERATION) {
    const auto constants = index_type->getAttribute(TypeKeyImpl::ENUMERATION_CONSTANTS);
    if (constants.has_value()) {
      count = static_cast<PascalInteger>(cast_by_enum<TypeKeyImpl::ENUMERATION_CONSTANTS>(constants).size());
    }
  } else {
    errorHandler()->flag(token, PascalErrorCode::INVALID_INDEX_TYPE, currentParser());
  }
  array_type->setAttribute(TypeKeyImpl::ARRAY_ELEMENT_COUNT, count);
}

std::shared_ptr<TypeSpecImplBase> ArrayTypeParser::parseElementType(std::shared_ptr<PascalToken> token)
{
  TypeSpecificationParser parser(currentParser());
  return parser.parseSpec(token);
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::leftBracketSet() {
  auto s = SimpleTypeParser::simpleTypeStartSet();
  s.insert({PascalTokenTypeImpl::LEFT_BRACKET,
              PascalTokenTypeImpl::RIGHT_BRACKET});
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::rightBracketSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::RIGHT_BRACKET,
    PascalTokenTypeImpl::OF,
    PascalTokenTypeImpl::SEMICOLON};
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::ofSet() {
  auto s = TypeSpecificationParser::typeStartSet();
  s.insert({PascalTokenTypeImpl::OF,
            PascalTokenTypeImpl::SEMICOLON});
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::indexStartSet() {
  auto s = SimpleTypeParser::simpleTypeStartSet();
  s.insert(PascalTokenTypeImpl::COMMA);
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::indexFollowSet() {
  auto s = ArrayTypeParser::indexStartSet();
  s.merge(ArrayTypeParser::indexEndSet());
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet ArrayTypeParser::indexEndSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::RIGHT_BRACKET,
    PascalTokenTypeImpl::OF,
    PascalTokenTypeImpl::SEMICOLON
  };
  return s;
}
