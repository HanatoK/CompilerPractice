#include "CaseStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "TypeChecker.h"
//#include "ExpressionExecutor.h"

CaseStatementParser::CaseStatementParser(const std::shared_ptr<PascalParserTopDown>& parent)
    : PascalSubparserTopDownBase(parent) {}

std::shared_ptr<ICodeNodeImplBase> CaseStatementParser::parse(
    std::shared_ptr<PascalToken> token,
    std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  // consume the CASE
  token = nextToken();
  // create a SELECT node
  auto select_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::SELECT));
  // parse the CASE expression
  ExpressionParser expression_parser(currentParser());
  auto expr_node = expression_parser.parse(token, parent_id);
  const auto expr_type = (expr_node != nullptr) ? expr_node->getTypeSpec() : Predefined::instance().undefinedType;
  // type check: the CASE expression's type must be integer, character, or enumeration
  if (!TypeChecker::TypeChecking::isInteger(expr_type) &&
      !TypeChecker::TypeChecking::isChar(expr_type) &&
      (expr_type->form() != TypeFormImpl::ENUMERATION)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  select_node->addChild(std::move(expr_node));
  // synchronize to OF
  token = synchronize(CaseStatementParser::ofSet());
  if (token->type() == PascalTokenTypeImpl::OF) {
    // consume OF
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_OF, currentParser());
  }
  // record the branch constants to avoid duplicates
  std::vector<VariableValueT> constant_set;
  // loop to parse each CASE until the END token
  while (!token->isEof() && token->type() != PascalTokenTypeImpl::END) {
    select_node->addChild(parseBranch(token, constant_set, expr_type));
    token = currentToken();
    const auto token_type = token->type();
    // look for the semicolon between CASE branches
    if (token_type == PascalTokenTypeImpl::SEMICOLON) {
      // consume ;
      token = nextToken();
    } else {
      // if start at the next constant, then missing ;
      if (CaseStatementParser::constantStartSet().contains(token_type)) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON,
                             currentParser());
      }
    }
  }
  // look for the END token
  if (token->type() == PascalTokenTypeImpl::END) {
    // consume the END
    nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_END, currentParser());
  }
  return select_node;
}

std::shared_ptr<ICodeNodeImplBase>
CaseStatementParser::parseBranch(std::shared_ptr<PascalToken> token,
                                 std::vector<VariableValueT> &constant_set,
                                 const std::shared_ptr<TypeSpecImplBase>& expression_type) {
  // create an SELECT_BRANCH node and a SELECT_CONSTANTS node
  auto branch_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::SELECT_BRANCH));
  auto constants_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::SELECT_CONSTANTS));
  parseConstantList(token, constants_node, constant_set, expression_type);
  // look for the : token
  token = currentToken();
  if (token->type() == PascalTokenTypeImpl::COLON) {
    // consume :
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_COLON,
                         currentParser());
  }
  branch_node->addChild(std::move(constants_node));
  // parse the branch statement
  StatementParser statement_parser(currentParser());
  branch_node->addChild(statement_parser.parse(token, nullptr));
  return branch_node;
}

void CaseStatementParser::parseConstantList(std::shared_ptr<PascalToken> token,
    std::shared_ptr<ICodeNodeImplBase>& constants_node,
    std::vector<VariableValueT> &constant_set, const std::shared_ptr<TypeSpecImplBase>& expression_type) {
  // loop to parse each constant
  const auto constant_start_set = CaseStatementParser::constantStartSet();
//  auto search = constant_start_set.find(token->type());
  while (constant_start_set.contains(token->type())) {
    // the constants list node adopts the constant node
    constants_node->addChild(parseConstant(token, constant_set, expression_type));
    // synchronize to the COMMA
    token = synchronize(CaseStatementParser::commaSet());
    // look for the COMMA
    if (token->type() == PascalTokenTypeImpl::COMMA) {
      // consume ,
      token = nextToken();
    } else {
      if (constant_start_set.contains(token->type())) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_COMMA,
                             currentParser());
      }
    }
  }
}

std::shared_ptr<ICodeNodeImplBase>
CaseStatementParser::parseConstant(std::shared_ptr<PascalToken> token,
                                   std::vector<VariableValueT> &constant_set,
                                   const std::shared_ptr<TypeSpecImplBase>& expression_type) {
  token = synchronize(CaseStatementParser::constantStartSet());
  std::shared_ptr<ICodeNodeImplBase> constant_node = nullptr;
  auto constant_type = Predefined::instance().undefinedType;
  auto sign = PascalTokenTypeImpl::UNKNOWN;
  if (token->type() == PascalTokenTypeImpl::PLUS ||
      token->type() == PascalTokenTypeImpl::MINUS) {
    sign = token->type();
    // consume the sign
    token = nextToken();
  }
  switch (token->type()) {
    case PascalTokenTypeImpl::IDENTIFIER: {
      constant_node = parseIdentifierConstant(token, sign);
      if (constant_node != nullptr) {
        constant_type = constant_node->getTypeSpec();
      }
      break;
    }
    case PascalTokenTypeImpl::INTEGER: {
      constant_node = parseIntegerConstant(token->text(), sign);
      constant_type = Predefined::instance().integerType;
      break;
    }
    case PascalTokenTypeImpl::STRING: {
      constant_node = parseCharacterConstant(
          token, std::get<std::string>(token->value()), sign);
      constant_type = Predefined::instance().charType;
      break;
    }
    default: {
      errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT,
                           currentParser());
      break;
    }
  }
  // check for reused constants
  // TODO: BUG: parse case statement correctly!!
  if (constant_node != nullptr) {
    const auto constant_value = constant_node->type() == ICodeNodeTypeImpl::NEGATE ?
                                VariableValueT(getNegateNodeValue(constant_node)) :
        constant_node->getAttribute<ICodeKeyTypeImpl::VALUE>();
    const bool in_constant_set = std::any_of(
        constant_set.begin(), constant_set.end(),
        [&](const auto &a) { return a == constant_value; });
    if (in_constant_set) {
      errorHandler()->flag(token, PascalErrorCode::CASE_CONSTANT_REUSED,
                           currentParser());
    } else {
      constant_set.push_back(constant_value);
    }
  }
  // type check: the constant type must be comparison compatible with the CASE expression type
  if (!TypeChecker::TypeCompatibility::areComparisonCompatible(expression_type, constant_type)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
  // consume the constant
  nextToken();
  constant_node->setTypeSpec(constant_type);
  return constant_node;
}

std::shared_ptr<ICodeNodeImplBase> CaseStatementParser::parseIdentifierConstant(const std::shared_ptr<PascalToken>& token,
                                             const PascalTokenTypeImpl sign) {
  std::shared_ptr<ICodeNodeImplBase> constant_node = nullptr;
  std::shared_ptr<TypeSpecImplBase> constant_type = nullptr;
  // lookup the identifier in the symbol table stack
  const auto name = boost::to_lower_copy(token->text());
  auto id = getSymbolTableStack()->lookup(name);
  // undefined
  if (id == nullptr) {
    id = getSymbolTableStack()->enterLocal(name);
    id->setDefinition(DefinitionImpl::UNDEFINED);
    id->setTypeSpec(Predefined::instance().undefinedType);
    errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
    return nullptr;
  }
  const auto definition_code = id->getDefinition();
  // constant identifier
  if ((definition_code == DefinitionImpl::CONSTANT) ||
      (definition_code == DefinitionImpl::ENUMERATION_CONSTANT)) {
    const auto constant_value = id->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
    constant_type = id->getTypeSpec();
    using namespace TypeChecker::TypeChecking;
    if ((sign != PascalTokenTypeImpl::UNKNOWN) && (!isInteger(constant_type))) {
      errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT, currentParser());
    }
    constant_node = createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT);
    constant_node->setAttribute<ICodeKeyTypeImpl::VALUE>(constant_value);
  }
  id->appendLineNumber(token->lineNum());
  if (constant_node != nullptr) {
    constant_node->setTypeSpec(constant_type);
  }
  return constant_node;
}

std::shared_ptr<ICodeNodeImplBase> CaseStatementParser::parseIntegerConstant(const std::string &value,
                                          const PascalTokenTypeImpl sign) {
  auto constant_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT));
  const auto int_value = (PascalInteger)std::stoll(value);
  constant_node->setAttribute<ICodeKeyTypeImpl::VALUE>(int_value);
  // this differs from the book!
  if (sign == PascalTokenTypeImpl::MINUS) {
    auto negate_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::NEGATE));
    negate_node->addChild(std::move(constant_node));
    return negate_node;
  } else {
    return constant_node;
  }
}

std::shared_ptr<ICodeNodeImplBase> CaseStatementParser::parseCharacterConstant(
    const std::shared_ptr<PascalToken>& token,
    const std::string &value,
    const PascalTokenTypeImpl sign) {
  if (sign != PascalTokenTypeImpl::UNKNOWN) {
    errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT,
                         currentParser());
    return nullptr;
  } else {
    if (value.size() != 1) {
      errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT,
                           currentParser());
      return nullptr;
    } else {
      auto constant_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::STRING_CONSTANT));
      constant_node->setAttribute<ICodeKeyTypeImpl::VALUE>(value);
      return constant_node;
    }
  }
}

PascalInteger CaseStatementParser::getNegateNodeValue(const std::shared_ptr<ICodeNodeImplBase>& node)
{
  if (node->type() == ICodeNodeTypeImpl::NEGATE) {
    // get the first child node
    const auto child_node = node->childrenBegin();
    const auto positive_value = std::get<PascalInteger>((*child_node)->getAttribute<ICodeKeyTypeImpl::VALUE>());
    const PascalInteger result = -1 * positive_value;
    return result;
  } else {
    return 0;
  }
}

PascalSubparserTopDownBase::TokenTypeSet CaseStatementParser::constantStartSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::INTEGER,
    PascalTokenTypeImpl::PLUS,
    PascalTokenTypeImpl::MINUS,
    PascalTokenTypeImpl::STRING
  };
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet CaseStatementParser::ofSet() {
  auto s = CaseStatementParser::constantStartSet();
  s.insert(PascalTokenTypeImpl::OF);
  s.merge(StatementParser::statementFollowSet());
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet CaseStatementParser::commaSet() {
  auto s = CaseStatementParser::constantStartSet();
  s.insert({PascalTokenTypeImpl::COMMA,
              PascalTokenTypeImpl::COLON});
  s.merge(StatementParser::statementStartSet());
  s.merge(StatementParser::statementFollowSet());
  return s;
}
