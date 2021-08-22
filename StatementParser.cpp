#include "StatementParser.h"
#include "IntermediateImpl.h"

StatementParser::StatementParser(PascalParserTopDown *parent)
    : PascalParserTopDown(parent->scanner(), parent) {}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
StatementParser::parse(std::shared_ptr<Token> token) {
  std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
      statement_node = nullptr;
  const auto pascal_token = dynamic_cast<PascalToken *>(token.get());
  switch (pascal_token->type()) {
  case PascalTokenType::BEGIN: {
    //      CompoundStatementParser compound_parser(this);
    //      statement_node = compound_parser.parse(token);
    //      break;
  }
  case PascalTokenType::IDENTIFIER: {
    //      AssignmentStatementParser assignment_parser(this);
    //      statement_node = assignment_parser.parse(token);
    //      break;
  }
  default: {
    ICodeNodeTypeImpl t(ICodeNodeTypeImpl::NO_OP);
    statement_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(t);
    break;
  }
  }
  return statement_node;
}

void StatementParser::setLineNumber(
    ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> *node,
    std::shared_ptr<Token> token) {
  if (node != nullptr) {
    //    ICodeKeyImpl k{ICodeKeyImpl::LINE};
    //    node->setAttribute();
  }
}
