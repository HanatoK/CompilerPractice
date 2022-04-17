#include "ExpressionParser.h"
#include "VariableParser.h"
#include "TypeChecker.h"

ExpressionParser::ExpressionParser(const std::shared_ptr<PascalParserTopDown>& parent):
  PascalSubparserTopDownBase(parent)
{

}

ExpressionParser::~ExpressionParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parse(std::shared_ptr<PascalToken> token)
{
  using namespace TypeChecker::TypeCompatibility;
  // parse a simple expression and make the root of its tree the root node
  auto root_node = std::shared_ptr(parseSimpleExpression(token));
  auto result_type = (root_node != nullptr) ? root_node->getTypeSpec() : Predefined::instance().undefinedType;
  token = currentToken();
  const auto token_type = token->type();
  const auto search = relOpsMap.find(token_type);
  if (search != relOpsMap.end()) {
    // relational operator found
    const auto node_type = relOpsMap.at(token_type);
    auto op_node = std::shared_ptr(createICodeNode(node_type));
    op_node->addChild(std::move(root_node));
    // now root_node should be nullptr
    // consume the operator
    token = nextToken();
    // parse the second simple expression
    auto simple_expr_node = std::shared_ptr(parseSimpleExpression(token));
    // type check: the operands must be comparison compatible
    const auto simple_expr_type = (simple_expr_node != nullptr) ? simple_expr_node->getTypeSpec() : Predefined::instance().undefinedType;
    if (areComparisonCompatible(result_type, simple_expr_type)) {
      result_type = Predefined::instance().booleanType;
    } else {
      errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
      result_type = Predefined::instance().undefinedType;
    }
    op_node->addChild(simple_expr_node);
    root_node = std::move(op_node);
    root_node->setTypeSpec(result_type);
  }
  return root_node;
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parseFactor(std::shared_ptr<PascalToken> token)
{
  std::shared_ptr<ICodeNodeImplBase> root_node = nullptr;
  auto token_type = token->type();
  auto symbol_table_stack = getSymbolTableStack();
  switch (token_type) {
    case PascalTokenTypeImpl::IDENTIFIER: {
      // lookup the identifier in the symbol table stack
      // flag the identifier as undefined if it's not found
      root_node = parseIdentifier(token);
      break;
    }
    case PascalTokenTypeImpl::INTEGER: {
      root_node = createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT);
      root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(token->value());
      root_node->setTypeSpec(Predefined::instance().integerType);
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::REAL: {
      root_node = createICodeNode(ICodeNodeTypeImpl::REAL_CONSTANT);
      root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(token->value());
      root_node->setTypeSpec(Predefined::instance().realType);
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::STRING: {
      const std::string s = any_to_string(token->value());
      root_node = createICodeNode(ICodeNodeTypeImpl::STRING_CONSTANT);
      root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(s);
      root_node->setTypeSpec(s.size() == 1 ? Predefined::instance().charType : createStringType(s));
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::NOT: {
      token = nextToken();
      root_node = createICodeNode(ICodeNodeTypeImpl::NOT);
      // parse the factor. The NOT node adopts it as child
      auto factor_node = parseFactor(token);
      if (factor_node != nullptr) {
        using namespace TypeChecker::TypeChecking;
        if (!isBoolean(factor_node->getTypeSpec())) {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        root_node->setTypeSpec(factor_node->getTypeSpec());
      } else {
        root_node->setTypeSpec(Predefined::instance().undefinedType);
      }
      root_node->addChild(factor_node);
      break;
    }
    case PascalTokenTypeImpl::LEFT_PAREN: {
      // consume the (
      token = nextToken();
      // parse the expression as the root node
      root_node = parse(token);
      if (root_node == nullptr) {
        root_node->setTypeSpec(Predefined::instance().undefinedType);
      }
      token = currentToken();
      if (token->type() == PascalTokenTypeImpl::RIGHT_PAREN) {
        // consume )
        token = nextToken();
      } else {
        errorHandler()->flag(token, PascalErrorCode::MISSING_RIGHT_PAREN, currentParser());
      }
      break;
    }
    default: {
      errorHandler()->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, currentParser());
      break;
    }
  }
  return root_node;
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parseTerm(std::shared_ptr<PascalToken> token)
{

  // parse a factor and make its node the root node
  auto root_node = parseFactor(token);
  auto result_type = (root_node != nullptr) ? root_node->getTypeSpec() : Predefined::instance().undefinedType;
  token = currentToken();
  auto token_type = token->type();
  while (multOpsMap.find(token_type) != multOpsMap.end()) {
    const auto op = token_type;
    const auto node_type = multOpsMap.at(op);
    auto op_node = std::shared_ptr(createICodeNode(node_type));
    op_node->addChild(std::move(root_node));
    token = nextToken();
    auto factor_node = parseFactor(token);
    const auto factor_type = (factor_node != nullptr) ? factor_node->getTypeSpec() : Predefined::instance().undefinedType;
    op_node->addChild(factor_node);
    using enum PascalTokenTypeImpl;
    using namespace TypeChecker::TypeChecking;
    switch (op) {
      case STAR: {
        if (areBothInteger(result_type, factor_type)) {
          // both are integer
          result_type = Predefined::instance().integerType;
        } else if (isAtLeastOneReal(result_type, factor_type)) {
          result_type = Predefined::instance().realType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      case SLASH: {
        // all integer and real operand combinations are real results
        if (areBothInteger(result_type, factor_type) ||
            isAtLeastOneReal(result_type, factor_type)) {
          result_type = Predefined::instance().realType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      case DIV:
      case MOD: {
        if (areBothInteger(result_type, factor_type)) {
          result_type = Predefined::instance().integerType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      case AND: {
        if (areBothBoolean(result_type, factor_type)) {
          result_type = Predefined::instance().booleanType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      default: {
        errorHandler()->flag(token, PascalErrorCode::INVALID_EXPRESSION, currentParser());
      }
    }
    root_node = std::move(op_node);
    root_node->setTypeSpec(result_type);
    token = currentToken();
    token_type = token->type();
  }
  return root_node;
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parseSimpleExpression(std::shared_ptr<PascalToken> token)
{
  using namespace TypeChecker::TypeChecking;
  auto token_type = token->type();
  auto sign_type = PascalTokenTypeImpl::UNKNOWN;
  // save the token for error reporting
  auto sign_token = token;
  // look for a leading + or - sign
  if ((token_type == PascalTokenTypeImpl::PLUS) ||
      (token_type == PascalTokenTypeImpl::MINUS)) {
    sign_type = token_type;
    sign_token = token;
    token = nextToken();
  }
  auto root_node = parseTerm(token);
  auto result_type = (root_node != nullptr) ? root_node->getTypeSpec() : Predefined::instance().undefinedType;
  // type check: leading sign
  if ((sign_type == PascalTokenTypeImpl::PLUS ||
       sign_type == PascalTokenTypeImpl::MINUS) &&
      isIntegerOrReal(result_type)) {
    errorHandler()->flag(sign_token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  // was there a leading minus sign?
  if (sign_type == PascalTokenTypeImpl::MINUS) {
    // create a NEGATE node and adopt the current tree
    auto negate_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::NEGATE));
    negate_node->setTypeSpec(root_node->getTypeSpec());
    negate_node->addChild(std::move(root_node));
    root_node = std::move(negate_node);
  }
  token = currentToken();
  token_type = token->type();
  // loop over additive operators
  while (addOpsMap.contains(token_type)) {
    const auto op = token_type;
    // create a new operator node and adopt the current tree
    // as its first child
    const auto node_type = addOpsMap.at(op);
    auto op_node = std::shared_ptr(createICodeNode(node_type));
    op_node->addChild(std::move(root_node));
    // consume the operator
    token = nextToken();
    // parse another term
    auto term_node = parseTerm(token);
    const auto term_type = (term_node != nullptr) ? term_node->getTypeSpec() : Predefined::instance().undefinedType;
    op_node->addChild(term_node);
    using enum PascalTokenTypeImpl;
    switch (op) {
      case PLUS:
      case MINUS: {
        if (areBothInteger(result_type, term_type)) {
          // both are integer
          result_type = Predefined::instance().integerType;
        } else if (isAtLeastOneReal(result_type, term_type)) {
          result_type = Predefined::instance().realType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      case OR: {
        if (areBothBoolean(result_type, term_type)) {
          result_type = Predefined::instance().booleanType;
        } else {
          errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
        }
        break;
      }
      default: {
        errorHandler()->flag(token, PascalErrorCode::INVALID_EXPRESSION, currentParser());
      }
    }
    // the operator node becomes the new root node
    root_node = std::move(op_node);
    root_node->setTypeSpec(result_type);
    token = currentToken();
    token_type = token->type();
  }
  return root_node;
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parseIdentifier(std::shared_ptr<PascalToken> token)
{
  // TODO: type checking
  std::shared_ptr<ICodeNodeImplBase> root_node = nullptr;
  auto symbol_table_stack = getSymbolTableStack();
  auto name = boost::algorithm::to_lower_copy(token->text());
  auto id = symbol_table_stack->lookup(name);
  if (id == nullptr) {
    errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
    id = symbol_table_stack->enterLocal(name);
    id->setDefinition(DefinitionImpl::UNDEFINED);
    id->setTypeSpec(Predefined::instance().undefinedType);
  }
  auto definition_code = id->getDefinition();
  switch (definition_code) {
    // TODO: type check
    case DefinitionImpl::CONSTANT: {
      const auto value = id->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
      const auto type_spec = id->getTypeSpec();
      if (std::holds_alternative<PascalInteger>(value)) {
        root_node = createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT);
        root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(value);
      } else if (std::holds_alternative<PascalFloat>(value)) {
        root_node = createICodeNode(ICodeNodeTypeImpl::REAL_CONSTANT);
        root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(value);
      } else if (std::holds_alternative<std::string>(value)) {
        root_node = createICodeNode(ICodeNodeTypeImpl::STRING_CONSTANT);
        root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(value);
      }
      id->appendLineNumber(token->lineNum());
      token = nextToken();
      if (root_node != nullptr) {
        root_node->setTypeSpec(type_spec);
      }
      break;
    }
    case DefinitionImpl::ENUMERATION_CONSTANT: {
      const auto value = id->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
      const auto type_spec = id->getTypeSpec();
      root_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT));
      root_node->setAttribute<ICodeKeyTypeImpl::VALUE>(value);
      id->appendLineNumber(token->lineNum());
      token = nextToken();
      root_node->setTypeSpec(type_spec);
      break;
    }
    default: {
      VariableParser parser(currentParser());
      root_node = parser.parseVariable(token, id);
      break;
    }
  }
//  auto result_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::VARIABLE));
//  result_node->setAttribute(ICodeKeyTypeImpl::ID, id);
//  id->appendLineNumber(token->lineNum());
//  token = nextToken();
//  return result_node;
  return root_node;
}

PascalSubparserTopDownBase::TokenTypeSet ExpressionParser::expressionStartSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::PLUS,
    PascalTokenTypeImpl::MINUS,
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::INTEGER,
    PascalTokenTypeImpl::REAL,
    PascalTokenTypeImpl::STRING,
    PascalTokenTypeImpl::NOT,
    PascalTokenTypeImpl::LEFT_PAREN
  };
  return s;
}
