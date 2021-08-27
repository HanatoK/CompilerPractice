#include "StatementParser.h"
#include "IntermediateImpl.h"
#include "CompoundStatementParser.h"
#include "AssignmentStatementParser.h"

StatementParser::StatementParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

StatementParser::~StatementParser()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
StatementParser::parse(std::shared_ptr<Token> token) {
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
      statement_node = nullptr;
  const auto pascal_token = dynamic_cast<PascalToken *>(token.get());
  switch (pascal_token->type()) {
  case PascalTokenType::BEGIN: {
    CompoundStatementParser compound_parser(*currentParser());
    statement_node = compound_parser.parse(token);
    break;
  }
  case PascalTokenType::IDENTIFIER: {
    AssignmentStatementParser assignment_parser(*currentParser());
    statement_node = assignment_parser.parse(token);
    break;
  }
  default: {
    ICodeNodeTypeImpl t(ICodeNodeTypeImpl::NO_OP);
    statement_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(t);
    break;
  }
  }
  setLineNumber(statement_node, token);
  return std::move(statement_node);
}

void StatementParser::setLineNumber(
    std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> &node,
    const std::shared_ptr<Token> &token) {
  if (node != nullptr) {
    node->setAttribute(ICodeKeyTypeImpl::LINE, token->lineNum());
  }
}

// TODO: figure out what does parseList do
void StatementParser::parseList(
    std::shared_ptr<Token> token,
    std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
        &parent_node,
    PascalTokenType terminator, PascalErrorCode error_code) {
  auto pascal_token = dynamic_cast<PascalToken*>(token.get());
//  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> statement_node = nullptr;
  while (!token->isEof() && pascal_token->type() != terminator) {
    auto statement_node = parse(token);
    parent_node->addChild(std::move(statement_node));
    token = currentToken();
    pascal_token = dynamic_cast<PascalToken*>(token.get());
    switch (pascal_token->type()) {
      // look for the semicolon between statements
      case PascalTokenType::SEMICOLON: {
        token = nextToken();
        pascal_token = dynamic_cast<PascalToken*>(token.get());
      }
      // if at the start of the next assignment statement,
      // then a semicolon is missing
      case PascalTokenType::IDENTIFIER: {
        errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
      }
      // unexpected token
      default: {
        if (pascal_token->type() == terminator) {
          errorHandler()->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, currentParser());
          token = nextToken();
          pascal_token = dynamic_cast<PascalToken*>(token.get());
        }
      }
    }
  }
  if (pascal_token->type() == terminator) {
    token = nextToken();
//    pascal_token = dynamic_cast<PascalToken*>(token.get());
  } else {
    errorHandler()->flag(token, error_code, currentParser());
  }
}
