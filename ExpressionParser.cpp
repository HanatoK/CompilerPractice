#include "ExpressionParser.h"

ExpressionParser::ExpressionParser(PascalParserTopDown &parent):
  PascalSubparserTopDownBase(parent),
  mRelOps({PascalTokenTypeImpl::EQUALS, PascalTokenTypeImpl::NOT_EQUALS,
           PascalTokenTypeImpl::LESS_THAN, PascalTokenTypeImpl::LESS_EQUALS,
           PascalTokenTypeImpl::GREATER_THAN, PascalTokenTypeImpl::GREATER_EQUALS}),
  mAddOps({PascalTokenTypeImpl::PLUS, PascalTokenTypeImpl::MINUS,
           PascalTokenTypeImpl::OR}),
  mRelOpsMap({{PascalTokenTypeImpl::EQUALS, ICodeNodeTypeImpl::EQ},
              {PascalTokenTypeImpl::NOT_EQUALS, ICodeNodeTypeImpl::NE},
              {PascalTokenTypeImpl::LESS_THAN, ICodeNodeTypeImpl::LT},
              {PascalTokenTypeImpl::LESS_EQUALS, ICodeNodeTypeImpl::LE},
              {PascalTokenTypeImpl::GREATER_THAN, ICodeNodeTypeImpl::GT},
              {PascalTokenTypeImpl::GREATER_EQUALS, ICodeNodeTypeImpl::GE}}),
  mAddOpsMap({{PascalTokenTypeImpl::PLUS, ICodeNodeTypeImpl::ADD},
              {PascalTokenTypeImpl::MINUS, ICodeNodeTypeImpl::SUBTRACT},
              {PascalTokenTypeImpl::OR, ICodeNodeTypeImpl::OR}})
{

}

ExpressionParser::~ExpressionParser()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ExpressionParser::parse(std::shared_ptr<PascalToken> token)
{
  // parse a simple expression and make the root of its tree the root node
  auto root_node = parseSimpleExpression(token);
  token = currentToken();
  auto token_type = token->type();
  auto search = mRelOps.find(token_type);
  if (search != mRelOps.end()) {
    // relational operator found
    auto node_type = mRelOpsMap[token_type];
    auto op_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(node_type);
    op_node->addChild(std::move(root_node));
    // now root_node should be nullptr
    // consume the operator
    token = nextToken();
    // parse the second simple expression
    op_node->addChild(parseSimpleExpression(token));
    root_node = std::move(op_node);
  }
  return std::move(root_node);
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ExpressionParser::parseSimpleExpression(std::shared_ptr<PascalToken> token)
{
  auto token_type = token->type();
  auto sign_type = (token_type == PascalTokenTypeImpl::PLUS || token_type == PascalTokenTypeImpl::MINUS) ? token_type : PascalTokenTypeImpl::UNKNOWN;
  // TODO
  return nullptr;
}
