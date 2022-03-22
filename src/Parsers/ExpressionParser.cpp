#include "ExpressionParser.h"

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
  // parse a simple expression and make the root of its tree the root node
  auto root_node = std::shared_ptr(parseSimpleExpression(token));
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
    op_node->addChild(parseSimpleExpression(token));
    root_node = std::move(op_node);
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
      auto name = boost::algorithm::to_lower_copy(token->text());
      auto id = symbol_table_stack->lookup(name);
      if (id == nullptr) {
        errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_UNDEFINED, currentParser());
        id = symbol_table_stack->enterLocal(name);
      }
      root_node = createICodeNode(ICodeNodeTypeImpl::VARIABLE);
      root_node->setAttribute(ICodeKeyTypeImpl::ID, id);
      id->appendLineNumber(token->lineNum());
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::INTEGER: {
      root_node = createICodeNode(ICodeNodeTypeImpl::INTEGER_CONSTANT);
      root_node->setAttribute(ICodeKeyTypeImpl::VALUE, token->value());
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::REAL: {
      root_node = createICodeNode(ICodeNodeTypeImpl::REAL_CONSTANT);
      root_node->setAttribute(ICodeKeyTypeImpl::VALUE, token->value());
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::STRING: {
      const std::string s = any_to_string(token->value());
      root_node = createICodeNode(ICodeNodeTypeImpl::STRING_CONSTANT);
      root_node->setAttribute(ICodeKeyTypeImpl::VALUE, s);
      token = nextToken();
      break;
    }
    case PascalTokenTypeImpl::NOT: {
      token = nextToken();
      root_node = createICodeNode(ICodeNodeTypeImpl::NOT);
      // parse the factor. The NOT node adopts it as child
      root_node->addChild(parseFactor(token));
      break;
    }
    case PascalTokenTypeImpl::LEFT_PAREN: {
      // consume the (
      token = nextToken();
      // parse the expression as the root node
      root_node = parse(token);
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
  token = currentToken();
  auto token_type = token->type();
  while (multOpsMap.find(token_type) != multOpsMap.end()) {
    const auto node_type = multOpsMap.at(token_type);
    auto op_node = std::shared_ptr(createICodeNode(node_type));
    op_node->addChild(std::move(root_node));
    token = nextToken();
    op_node->addChild(parseFactor(token));
    root_node = std::move(op_node);
    token = currentToken();
    token_type = token->type();
  }
  return root_node;
}

std::shared_ptr<ICodeNodeImplBase> ExpressionParser::parseSimpleExpression(std::shared_ptr<PascalToken> token)
{
  auto token_type = token->type();
  auto sign_type = PascalTokenTypeImpl::UNKNOWN;
  // look for a leading + or - sign
  if ((token_type == PascalTokenTypeImpl::PLUS) ||
      (token_type == PascalTokenTypeImpl::MINUS)) {
    sign_type = token_type;
    token = nextToken();
  }
  auto root_node = parseTerm(token);
  if (sign_type == PascalTokenTypeImpl::MINUS) {
    // create a NEGATE node and adopt the current tree
    auto negate_node = std::shared_ptr(createICodeNode(ICodeNodeTypeImpl::NEGATE));
    negate_node->addChild(std::move(root_node));
    root_node = std::move(negate_node);
  }
  token = currentToken();
  token_type = token->type();
  // loop over additive operators
  while (addOpsMap.find(token_type) != addOpsMap.end()) {
    // create a new operator node and adopt the current tree
    // as its first child
    const auto node_type = addOpsMap.at(token_type);
    auto op_node = std::shared_ptr(createICodeNode(node_type));
    op_node->addChild(std::move(root_node));
    // consume the operator
    token = nextToken();
    op_node->addChild(parseTerm(token));
    // the operator node becomes the new root node
    root_node = std::move(op_node);
    token = currentToken();
    token_type = token->type();
  }
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
