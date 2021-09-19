#include "StatementParser.h"
#include "IntermediateImpl.h"
#include "CompoundStatementParser.h"
#include "AssignmentStatementParser.h"

StatementParser::StatementParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

StatementParser::~StatementParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
StatementParser::parse(std::shared_ptr<PascalToken> token) {
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
      statement_node = nullptr;
  switch (token->type()) {
  case PascalTokenTypeImpl::BEGIN: {
    CompoundStatementParser compound_parser(*currentParser());
    statement_node = compound_parser.parse(token);
    break;
  }
  case PascalTokenTypeImpl::IDENTIFIER: {
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
    const std::shared_ptr<PascalToken> &token) {
  if (node != nullptr) {
    node->setAttribute(ICodeKeyTypeImpl::LINE, token->lineNum());
  }
}

void StatementParser::parseList(
    std::shared_ptr<PascalToken> token,
    std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
        &parent_node,
    PascalTokenTypeImpl terminator, PascalErrorCode error_code) {
  while (!token->isEof() && token->type() != terminator) {
    auto statement_node = parse(token);
    parent_node->addChild(std::move(statement_node));
    token = currentToken();
    const auto token_type = token->type();
    if (token_type == PascalTokenTypeImpl::SEMICOLON) {
      token = nextToken();
    } else if (token_type == PascalTokenTypeImpl::IDENTIFIER) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    } else if (token_type != terminator) {
      errorHandler()->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, currentParser());
      token = nextToken();
    }
  }
  if (token->type() == terminator) {
    token = nextToken();
  } else {
    errorHandler()->flag(token, error_code, currentParser());
  }
}
