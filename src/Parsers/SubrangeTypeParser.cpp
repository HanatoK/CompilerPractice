#include "SubrangeTypeParser.h"
#include "ConstantDefinitionsParser.h"

SubrangeTypeParser::SubrangeTypeParser(const std::shared_ptr<PascalParserTopDown>& parent): PascalSubparserTopDownBase(parent)
{

}

SubrangeTypeParser::~SubrangeTypeParser()
{

}

std::unique_ptr<TypeSpecImplBase> SubrangeTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  auto subrange_type = createType(TypeFormImpl::SUBRANGE);
  // parse the minimum constant
  decltype(token) constant_token = token->clone();
  ConstantDefinitionsParser parser(currentParser());
  auto min_val = parser.parseConstant(token);
  // set the minimum type
  const auto min_type = (constant_token->type() == PascalTokenTypeImpl::IDENTIFIER) ?
                  parser.getConstantType(constant_token):
                  parser.getConstantType(min_val);
  min_val = checkValueType(constant_token, min_val, min_type);
  std::any max_val;
  token = currentToken();
  bool saw_dot_dot = false;
  // look for the .. token
  if (token->type() == PascalTokenTypeImpl::DOT_DOT) {
    token = nextToken();
    saw_dot_dot = true;
  }
  auto token_type = token->type();
  if (ConstantDefinitionsParser::constantStartSet().contains(token_type)) {
    if (!saw_dot_dot) {
      errorHandler()->flag(constant_token, PascalErrorCode::MISSING_DOT_DOT, currentParser());
    }
    token = synchronize(ConstantDefinitionsParser::constantStartSet());
    constant_token = token->clone();
    max_val = parser.parseConstant(token);
    const auto max_type = (constant_token->type() == PascalTokenTypeImpl::IDENTIFIER) ?
                      parser.getConstantType(constant_token):
                      parser.getConstantType(max_val);
    max_val = checkValueType(constant_token, max_val, max_type);
    // are the type valid?
    if (min_type == nullptr || max_type == nullptr) {
      errorHandler()->flag(constant_token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
    } else if (min_type != max_type) { // is this comparison valid?
      errorHandler()->flag(constant_token, PascalErrorCode::INVALID_SUBRANGE_TYPE, currentParser());
    } else if (min_val.has_value() && max_val.has_value()) {
      if (std::any_cast<PascalInteger>(min_val) >= std::any_cast<PascalInteger>(max_val)) {
        errorHandler()->flag(constant_token, PascalErrorCode::MIN_GT_MAX, currentParser());
//        std::cout << "Min value: " << std::any_cast<PascalInteger>(min_val) << " "
//                     "Max value: " << std::any_cast<PascalInteger>(max_val) << std::endl;
      }
    }
  } else {
    errorHandler()->flag(constant_token, PascalErrorCode::INVALID_SUBRANGE_TYPE, currentParser());
  }
  subrange_type->setAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE, min_type);
  subrange_type->setAttribute(TypeKeyImpl::SUBRANGE_MIN_VALUE, min_val);
  subrange_type->setAttribute(TypeKeyImpl::SUBRANGE_MAX_VALUE, max_val);
  return subrange_type;
}

std::any SubrangeTypeParser::checkValueType(
    const std::shared_ptr<PascalToken>& token,
    const std::any& value, const std::shared_ptr<TypeSpecImplBase>& type)
{
  if (type == nullptr || type == Predefined::instance().integerType) return value;
  if (type == Predefined::instance().integerType) return value;
  else if (type == Predefined::instance().charType) {
    const auto ch = std::any_cast<std::string>(value)[0];
    return PascalInteger(ch);
  } else if (type->form() == TypeFormImpl::ENUMERATION) {
    return value;
  } else {
    errorHandler()->flag(token, PascalErrorCode::INVALID_SUBRANGE_TYPE, currentParser());
    return value;
  }
}
