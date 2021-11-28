#include "StatementParser.h"
#include "IntermediateImpl.h"
#include "CompoundStatementParser.h"
#include "AssignmentStatementParser.h"
#include "RepeatStatementParser.h"
#include "WhileStatementParser.h"
#include "IfStatementParser.h"
#include "CaseStatementParser.h"
#include "ForStatementParser.h"

StatementParser::StatementParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

StatementParser::~StatementParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::unique_ptr<ICodeNodeImplBase> StatementParser::parse(std::shared_ptr<PascalToken> token) {
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
    case PascalTokenTypeImpl::REPEAT: {
      RepeatStatementParser repeat_parser(*currentParser());
      statement_node = repeat_parser.parse(token);
      break;
    }
    case PascalTokenTypeImpl::WHILE: {
      WhileStatementParser while_parser(*currentParser());
      statement_node = while_parser.parse(token);
      break;
    }
    case PascalTokenTypeImpl::FOR: {
      ForStatementParser for_parser(*currentParser());
      statement_node = for_parser.parse(token);
      break;
    }
    case PascalTokenTypeImpl::IF: {
      IfStatementParser if_parser(*currentParser());
      statement_node = if_parser.parse(token);
      break;
    }
    case PascalTokenTypeImpl::CASE: {
      CaseStatementParser case_parser(*currentParser());
      statement_node = case_parser.parse(token);
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

void StatementParser::parseList(std::shared_ptr<PascalToken> token,
    std::unique_ptr<ICodeNodeImplBase>& parent_node,
    const PascalTokenTypeImpl& terminator, const PascalErrorCode& error_code) {
  auto terminator_set = statementStartSet;
  terminator_set.insert(terminator);
  while (!token->isEof() && token->type() != terminator) {
    auto statement_node = parse(token);
    parent_node->addChild(std::move(statement_node));
    token = currentToken();
    const auto token_type = token->type();
    if (token_type == PascalTokenTypeImpl::SEMICOLON) {
      token = nextToken();
    } else {
      // if at the start of the next statement, then missing a semicolon
      const auto search = statementStartSet.find(token_type);
      if (search != statementStartSet.end()) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
      }
    }
    // synchromize at the start of the next statement
    token = synchronize(terminator_set);
    /*else if (token_type == PascalTokenTypeImpl::IDENTIFIER) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    } else if (token_type != terminator) {
      errorHandler()->flag(token, PascalErrorCode::UNEXPECTED_TOKEN, currentParser());
      token = nextToken();
    }*/
  }
  if (token->type() == terminator) {
    token = nextToken();
  } else {
    errorHandler()->flag(token, error_code, currentParser());
  }
}
