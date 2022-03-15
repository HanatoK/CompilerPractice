#include "CaseStatementParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"

CaseStatementParser::CaseStatementParser(const std::shared_ptr<PascalParserTopDown>& parent)
    : PascalSubparserTopDownBase(parent) {}

std::shared_ptr<ICodeNodeImplBase> CaseStatementParser::parse(std::shared_ptr<PascalToken> token) {
  // consume the CASE
  token = nextToken();
  // create a SELECT node
  std::shared_ptr<ICodeNodeImplBase> select_node
      = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(ICodeNodeTypeImpl::SELECT);
  // parse the CASE expression
  // I use the expression parser here, which is different from the book.
  // Maybe I need something like constant expression in C++?
  ExpressionParser expression_parser(currentParser());
  select_node->addChild(expression_parser.parse(token));
  // synchronize to OF
  token = synchronize(CaseStatementParser::ofSet());
  if (token->type() == PascalTokenTypeImpl::OF) {
    // consume OF
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_OF, currentParser());
  }
  // record the branch constants to avoid duplicates
  std::vector<std::any> constant_set;
  // loop to parse each CASE until the END token
  while (!token->isEof() && token->type() != PascalTokenTypeImpl::END) {
    select_node->addChild(parseBranch(token, constant_set));
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
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_END, currentParser());
  }
  return select_node;
}

std::unique_ptr<ICodeNodeImplBase>
CaseStatementParser::parseBranch(std::shared_ptr<PascalToken> token,
                                 std::vector<std::any> &constant_set) {
  // create an SELECT_BRANCH node and a SELECT_CONSTANTS node
  auto branch_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
      ICodeNodeTypeImpl::SELECT_BRANCH);
  auto constants_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
      ICodeNodeTypeImpl::SELECT_CONSTANTS);
  parseConstantList(token, constants_node, constant_set);
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
  branch_node->addChild(statement_parser.parse(token));
  return branch_node;
}

void CaseStatementParser::parseConstantList(std::shared_ptr<PascalToken> token,
    std::unique_ptr<ICodeNodeImplBase>& constants_node,
    std::vector<std::any> &constant_set) {
  // loop to parse each constant
  const auto constant_start_set = CaseStatementParser::constantStartSet();
//  auto search = constant_start_set.find(token->type());
  while (constant_start_set.contains(token->type())) {
    // the constants list node adopts the constant node
    constants_node->addChild(parseConstant(token, constant_set));
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

std::unique_ptr<ICodeNodeImplBase>
CaseStatementParser::parseConstant(std::shared_ptr<PascalToken> token,
                                   std::vector<std::any> &constant_set) {
  token = synchronize(CaseStatementParser::constantStartSet());
  std::unique_ptr<ICodeNodeImplBase> constant_node = nullptr;
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
    break;
  }
  case PascalTokenTypeImpl::INTEGER: {
    constant_node = parseIntegerConstant(token->text(), sign);
    break;
  }
  case PascalTokenTypeImpl::STRING: {
    constant_node = parseCharacterConstant(
        token, std::any_cast<std::string>(token->value()), sign);
    break;
  }
  default: {
    errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT,
                         currentParser());
    break;
  }
  }
  // check for reused constants
  if (constant_node != nullptr) {
    if (constant_node->type() == ICodeNodeTypeImpl::NEGATE) {

    }
    const auto constant_value = constant_node->type() == ICodeNodeTypeImpl::NEGATE ?
        std::any(getNegateNodeValue(constant_node)) :
        constant_node->getAttribute(ICodeKeyTypeImpl::VALUE);
    const bool in_constant_set = std::any_of(
        constant_set.begin(), constant_set.end(),
        [&](const auto &a) { return compare_any(a, constant_value); });
    if (in_constant_set) {
      errorHandler()->flag(token, PascalErrorCode::CASE_CONSTANT_REUSED,
                           currentParser());
    } else {
      constant_set.push_back(constant_value);
    }
  }
  // consume the constant
  nextToken();
  return constant_node;
}

std::unique_ptr<ICodeNodeImplBase>
CaseStatementParser::parseIdentifierConstant(const std::shared_ptr<PascalToken>& token,
                                             const PascalTokenTypeImpl sign) {
  // don't allow for now
  errorHandler()->flag(token, PascalErrorCode::INVALID_CONSTANT,
                       currentParser());
  return nullptr;
}

std::unique_ptr<ICodeNodeImplBase> CaseStatementParser::parseIntegerConstant(const std::string &value,
                                          const PascalTokenTypeImpl sign) {
  auto constant_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
      ICodeNodeTypeImpl::INTEGER_CONSTANT);
  const auto int_value = std::stoll(value);
  constant_node->setAttribute(ICodeKeyTypeImpl::VALUE, int_value);
  // this differs from the book!
  if (sign == PascalTokenTypeImpl::MINUS) {
    auto negate_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
        ICodeNodeTypeImpl::NEGATE);
    negate_node->addChild(std::move(constant_node));
    return negate_node;
  } else {
    return constant_node;
  }
}

std::unique_ptr<ICodeNodeImplBase> CaseStatementParser::parseCharacterConstant(
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
      auto constant_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(
          ICodeNodeTypeImpl::STRING_CONSTANT);
      constant_node->setAttribute(ICodeKeyTypeImpl::VALUE, value);
      return constant_node;
    }
  }
}

PascalInteger CaseStatementParser::getNegateNodeValue(const std::unique_ptr<ICodeNodeImplBase> &node)
{
  if (node->type() == ICodeNodeTypeImpl::NEGATE) {
    // get the first child node
    const auto child_node = node->childrenBegin();
    const auto positive_value = std::any_cast<PascalInteger>((*child_node)->getAttribute(ICodeKeyTypeImpl::VALUE));
    // TODO: possible overflow
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
