#include "ConstantDefinitionsParser.h"
#include "ExpressionParser.h"

ConstantDefinitionsParser::ConstantDefinitionsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

ConstantDefinitionsParser::~ConstantDefinitionsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ConstantDefinitionsParser::parse(std::shared_ptr<PascalToken> token)
{
  token = synchronize(identifierSet);
  // loop to parse a sequence of constant definitions
  while (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    const auto name = boost::algorithm::to_lower_copy(token->text());
    auto constant_id = getSymbolTableStack()->lookupLocal(name);
    // enter the new identifier into the symbol table
    if (constant_id == nullptr) {
      constant_id = getSymbolTableStack()->enterLocal(name);
      constant_id->appendLineNumber(token->lineNum());
    } else {
      errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_REDEFINED, currentParser());
      constant_id = nullptr;
    }
    // consume the identifier token
    nextToken();
    // synchronize on the = token
    token = synchronize(equalsSet);
    if (token->type() == PascalTokenTypeImpl::EQUALS) {
      // consume =
      token = nextToken();
    } else {
      errorHandler()->flag(token, PascalErrorCode::MISSING_EQUALS, currentParser());
    }
    // parse the constant value
    // TODO: Is this shallow-copy correct?
    const auto current_token = token;
    const std::any constant_value = parseConstant(token);
    // set identifier as a constant and set its value
    if (constant_id != nullptr) {
      constant_id->setDefinition(DefinitionImpl::CONSTANT);
      constant_id->setAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE, constant_value);
      // set the constant's type
      auto constant_type = (current_token->type() == PascalTokenTypeImpl::IDENTIFIER) ?
                           getConstantType(current_token) : getConstantType(constant_value);
      constant_id->setTypeSpec(constant_type);
    }
    token = currentToken();
    const auto token_type = token->type();
    // look for one or more semicolons after a definition
    if (token_type == PascalTokenTypeImpl::SEMICOLON) {
      // consume all the semicolons
      while (token->type() == PascalTokenTypeImpl::SEMICOLON) {
        token = nextToken();
      }
    } else if (nextStartSet.contains(token_type)) {
      // if at the start of the next definition or declaration,
      // then missing a semicolon
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    }
    token = synchronize(identifierSet);
  }
  return nullptr;
}

std::any ConstantDefinitionsParser::parseConstant(std::shared_ptr<PascalToken>& token)
{
  // -1 for negative, +1 for positive
  int sign = 1;
  // synchronize at the start of a constant
  token = synchronize(constantStartSet);
  auto token_type = token->type();
  if (token_type == PascalTokenTypeImpl::PLUS ||
      token_type == PascalTokenTypeImpl::MINUS) {
    sign = (token_type == PascalTokenTypeImpl::PLUS) ? 1 : -1;
    // consume the sign
    token = nextToken();
  }
  // parse the constant
  switch (token->type()) {
    case PascalTokenTypeImpl::IDENTIFIER: {
      return parseIdentifierConstant(token, sign);
    }
    case PascalTokenTypeImpl::INTEGER: {
      // integers are stored as long long
      const long long val = sign * std::any_cast<long long>(token->value());
      nextToken();
      return val;
    }
    case PascalTokenTypeImpl::REAL: {
      const double val = sign * std::any_cast<double>(token->value());
      nextToken();
      return val;
    }
    case PascalTokenTypeImpl::STRING: {
      if (sign != 0) {
        errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT, currentParser());
      }
      nextToken();
      return token->value();
    }
    default: {
      errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT, currentParser());
      return std::any();
    }
  }
}

std::any ConstantDefinitionsParser::parseIdentifierConstant(std::shared_ptr<PascalToken>& token, int sign)
{
  auto name = token->text();
  auto id = getSymbolTableStack()->lookup(name);
  // consume the identifier
  nextToken();
  if (id == nullptr) {
    errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
    return std::make_tuple(PascalTokenTypeImpl::UNKNOWN, std::any());
  }
  auto definition = id->getDefinition();
  if (definition == DefinitionImpl::CONSTANT) {
    const auto constant_value = id->getAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE);
    id->appendLineNumber(token->lineNum());
    if (any_is<long long>(constant_value)) {
      return sign * std::any_cast<long long>(constant_value);
    } else if (any_is<double>(constant_value)) {
      return sign * std::any_cast<double>(constant_value);
    } else if (any_is<std::string>(constant_value)) {
      return constant_value;
    } else {
      return std::any();
    }
  } else {
    return std::any();
  }
}

std::shared_ptr<TypeSpecImplBase> ConstantDefinitionsParser::getConstantType(const std::any& value) const
{
  std::shared_ptr<TypeSpecImplBase> constant_type = nullptr;
  if (any_is<long long>(value)) {
    constant_type = Predefined::instance().integerType;
  } else if (any_is<double>(value)) {
    constant_type = Predefined::instance().realType;
  } else if (any_is<std::string>(value)) {
    const auto s = std::any_cast<std::string>(value);
    if (s.size() == 1) constant_type = Predefined::instance().charType;
    else constant_type = createStringType(s);
  }
  return constant_type;
}

std::shared_ptr<TypeSpecImplBase> ConstantDefinitionsParser::getConstantType(std::shared_ptr<PascalToken>& token)
{
  auto id = getSymbolTableStack()->lookup(token->text());
  if (id == nullptr) return nullptr;
  auto definition = id->getDefinition();
  if (definition == DefinitionImpl::CONSTANT ||
      definition == DefinitionImpl::ENUMERATION_CONSTANT) {
    return id->getTypeSpec();
  } else {
    return nullptr;
  }
}
