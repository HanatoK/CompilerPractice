#include "SubrangeTypeParser.h"
#include "IntermediateImpl.h"
#include "ConstantDefinitionsParser.h"

SubrangeTypeParser::SubrangeTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

SubrangeTypeParser::~SubrangeTypeParser()
{

}

std::unique_ptr<TypeSpecImplBase> SubrangeTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  auto subrange_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::SUBRANGE);
  // parse the minimum constant
  auto constant_token = *token;
  ConstantDefinitionsParser parser(*currentParser());
  auto min_val = parser.parseConstant(token);
  // set the minimum type
  auto min_type = (constant_token.type() == PascalTokenTypeImpl::IDENTIFIER) ?
                  parser.getConstantType(constant_token):
                  parser.getConstantType(min_val);
  min_val = checkValueType(constant_token, min_val, min_type);
  token = currentToken();
  bool saw_dot_dot = false;
  // look for the .. token
  if (token->type() == PascalTokenTypeImpl::DOT_DOT) {
    token = nextToken();
    saw_dot_dot = true;
  }
  auto token_type = token->type();
  if (constantStartSet.contains(token_type)) {
    if (!saw_dot_dot) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_DOT_DOT, currentParser());
    }
    token = synchronize(constantStartSet);
    constant_token = *token;
    auto max_val = parser.parseConstant(token);
    auto max_type = (constant_token.type() == PascalTokenTypeImpl::IDENTIFIER) ?
                      parser.getConstantType(constant_token):
                      parser.getConstantType(max_val);
    max_val = checkValueType(constant_token, max_val, max_type);
    // are the type valid?
    if (min_type == nullptr || max_type == nullptr) {
      errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
    } else if (min_type != max_type) { // is this comparison valid?
      // TODO
    }
  }
}
