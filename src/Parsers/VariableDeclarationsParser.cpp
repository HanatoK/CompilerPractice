#include "VariableDeclarationsParser.h"
#include "DeclarationsParser.h"
#include "TypeSpecificationParser.h"

VariableDeclarationsParser::VariableDeclarationsParser(const std::shared_ptr<PascalParserTopDown>& parent)
  : PascalSubparserTopDownBase(parent), mDefinition(DefinitionImpl::UNDEFINED) {}

VariableDeclarationsParser::~VariableDeclarationsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::unique_ptr<ICodeNodeImplBase> VariableDeclarationsParser::parse(std::shared_ptr<PascalToken> token)
{
  token = synchronize(VariableDeclarationsParser::identifierSet());
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
    } else if (VariableDeclarationsParser::nextStartSet().contains(token_type)) {
      // if at the start of the next definition or declaration, then a missing semicolon
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    }
    token = synchronize(VariableDeclarationsParser::identifierSet());
  }
  return nullptr;
}

std::vector<std::shared_ptr<SymbolTableEntryImplBase>>
VariableDeclarationsParser::parseIdentifierSublist(std::shared_ptr<PascalToken> token) {
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> sub_list;
  do {
    token = synchronize(VariableDeclarationsParser::identiferStartSet());
    auto id = parseIdentifier(token);
    if (id != nullptr) {
      sub_list.push_back(id);
    }
    token = synchronize(VariableDeclarationsParser::commaSet());
    // look for ,
    auto token_type = token->type();
    if (token_type == PascalTokenTypeImpl::COMMA) {
      token = nextToken();
      if (VariableDeclarationsParser::identiferFollowSet().contains(token->type())) {
        errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
      }
    } else if (VariableDeclarationsParser::identiferStartSet().contains(token_type)) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_COMMA, currentParser());
    }
  } while (!VariableDeclarationsParser::identiferFollowSet().contains(token->type()));
  // parse the type specification
  auto type_spec = parseTypeSpec(token);
  for (auto& elem : sub_list) {
    elem->setTypeSpec(type_spec);
  }
  return sub_list;
}

void VariableDeclarationsParser::setDefinition(const DefinitionImpl& definition) {
  mDefinition = definition;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::identifierSet() {
  auto s = DeclarationsParser::varStartSet();
  s.insert({
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::END,
    PascalTokenTypeImpl::SEMICOLON
  });
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::nextStartSet() {
  auto s = DeclarationsParser::routineStartSet();
  s.insert({
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::SEMICOLON
  });
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::identiferStartSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::COMMA
  };
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::identiferFollowSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::COLON,
    PascalTokenTypeImpl::SEMICOLON
  };
  s.merge(DeclarationsParser::varStartSet());
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::commaSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::COMMA,
    PascalTokenTypeImpl::COLON,
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::SEMICOLON
  };
  return s;
}

std::shared_ptr<SymbolTableEntryImplBase>
VariableDeclarationsParser::parseIdentifier(std::shared_ptr<PascalToken> token) {
  std::shared_ptr<SymbolTableEntryImplBase> id = nullptr;
  if (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    auto name = boost::algorithm::to_lower_copy(token->text());
    id = getSymbolTableStack()->lookupLocal(name);
    if (id == nullptr) {
      // if the identifier is not found,
      // insert it into the symbol table
      id = getSymbolTableStack()->enterLocal(name);
      // TODO: possible bug of design
      // mDefinition may be undefined after constructor
      id->setDefinition(mDefinition);
      id->appendLineNumber(token->lineNum());
    } else {
      errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_REDEFINED, currentParser());
    }
    // consume the identifier
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
  }
  return id;
}

std::shared_ptr<TypeSpecImplBase> VariableDeclarationsParser::parseTypeSpec(std::shared_ptr<PascalToken> token) {
  token = synchronize(VariableDeclarationsParser::colonSet());
  if (token->type() == PascalTokenTypeImpl::COLON) {
    // consume :
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_COLON, currentParser());
  }
  // parse the type specification
  TypeSpecificationParser parser(currentParser());
  auto type_spec = parser.parseSpec(token);
  return type_spec;
}

PascalSubparserTopDownBase::TokenTypeSet VariableDeclarationsParser::colonSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::COLON,
    PascalTokenTypeImpl::SEMICOLON
  };
  return s;
}
