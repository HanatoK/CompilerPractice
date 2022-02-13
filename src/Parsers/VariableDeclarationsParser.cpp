#include "VariableDeclarationsParser.h"

VariableDeclarationsParser::VariableDeclarationsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

VariableDeclarationsParser::~VariableDeclarationsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::unique_ptr<ICodeNodeImplBase> VariableDeclarationsParser::parse(std::shared_ptr<PascalToken> token)
{
  token = synchronize(variableDeclarationsIdentifierSet);
  // loop to parse a sequence of variable declarations separated by semicolons
  while (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    // parse the identifier sublist and its type specification
    parseIdentifierSublist(token);
    token = currentToken();
    const auto token_type = token->type();
    // look for one or more semicolons after a definition
    if (token_type == PascalTokenTypeImpl::SEMICOLON) {
      // consume the ;
      while (token->type() == PascalTokenTypeImpl::SEMICOLON) {
        token = nextToken();
      }
    } else if (nextStartSet.contains(token_type)) {
      // if at the start of the next definition or declaration, then a missing semicolon
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    }
    token = synchronize(variableDeclarationsIdentifierSet);
  }
}

void VariableDeclarationsParser::parseIdentifierSublist(std::shared_ptr<PascalToken> token) {

}

void VariableDeclarationsParser::setDefinition(const DefinitionImpl& definition) {
  mDefinition = definition;
}
