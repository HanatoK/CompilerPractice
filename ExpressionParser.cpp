#include "ExpressionParser.h"

ExpressionParser::ExpressionParser(PascalParserTopDown &parent):
  PascalSubparserTopDownBase(parent),
  mRelOps({PascalTokenType::EQUALS, PascalTokenType::NOT_EQUALS,
           PascalTokenType::LESS_THAN, PascalTokenType::LESS_EQUALS,
           PascalTokenType::GREATER_THAN, PascalTokenType::GREATER_EQUALS}),
  mAddOps({PascalTokenType::PLUS, PascalTokenType::MINUS,
           PascalTokenType::OR}),
  mRelOpsMap({{PascalTokenType::EQUALS, ICodeNodeTypeImpl::EQ},
              {PascalTokenType::NOT_EQUALS, ICodeNodeTypeImpl::NE},
              {PascalTokenType::LESS_THAN, ICodeNodeTypeImpl::LT},
              {PascalTokenType::LESS_EQUALS, ICodeNodeTypeImpl::LE},
              {PascalTokenType::GREATER_THAN, ICodeNodeTypeImpl::GT},
              {PascalTokenType::GREATER_EQUALS, ICodeNodeTypeImpl::GE}}),
  mAddOpsMap({{PascalTokenType::PLUS, ICodeNodeTypeImpl::ADD},
              {PascalTokenType::MINUS, ICodeNodeTypeImpl::SUBTRACT},
              {PascalTokenType::OR, ICodeNodeTypeImpl::OR}})
{

}

ExpressionParser::~ExpressionParser()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ExpressionParser::parse(std::shared_ptr<Token> token)
{
  // parse a simple expression and make the root of its tree the root node
  auto root_node = parseSimpleExpression(token);
  token = currentToken();
  auto pascal_token = dynamic_cast<PascalToken*>(token.get());
  auto token_type = pascal_token->type();
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

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > ExpressionParser::parseSimpleExpression(std::shared_ptr<Token> token)
{
  auto pascal_token = dynamic_cast<PascalToken*>(token.get());
  auto token_type = pascal_token->type();
  auto sign_type = (token_type == PascalTokenType::PLUS || token_type == PascalTokenType::MINUS) ? token_type : PascalTokenType::UNKNOWN;
  // TODO
  return nullptr;
}
