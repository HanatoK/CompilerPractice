#include "TypeDefinitionsParser.h"
#include "TypeSpecificationParser.h"
#include "DeclarationsParser.h"
#include "ConstantDefinitionsParser.h"

TypeDefinitionsParser::TypeDefinitionsParser(const std::shared_ptr<PascalParserTopDown>& parent)
  : PascalSubparserTopDownBase(parent) {}

TypeDefinitionsParser::~TypeDefinitionsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
}

std::shared_ptr<ICodeNodeImplBase> TypeDefinitionsParser::parse(
    std::shared_ptr<PascalToken> token,
    std::shared_ptr<SymbolTableEntryImplBase> parent_id)
{
  token = synchronize(TypeDefinitionsParser::identifierSet());
  // loop to parse a sequence of type definitions
  while (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    auto name = boost::algorithm::to_lower_copy(token->text());
    auto type_id = getSymbolTableStack()->lookupLocal(name);
    if (type_id == nullptr) {
      // enter the new identifier into the symbol table
      // but don't set how it's defined yet
      type_id = getSymbolTableStack()->enterLocal(name);
      type_id->appendLineNumber(token->lineNum());
    } else {
      errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_REDEFINED, currentParser());
      type_id = nullptr;
    }
    // consume the identifier token
    token = nextToken();
    token = synchronize(TypeDefinitionsParser::equalsSet());
    if (token->type() == PascalTokenTypeImpl::EQUALS) {
      // consume =
      token = nextToken();
    } else {
      errorHandler()->flag(token, PascalErrorCode::MISSING_EQUALS, currentParser());
    }
    // parse the type specification
    TypeSpecificationParser type_spec_parser(currentParser());
    std::shared_ptr<TypeSpecImplBase> type_spec = type_spec_parser.parseSpec(token);
    if (type_id != nullptr) {
      type_id->setDefinition(DefinitionImpl::TYPE);
    }
    // cross-link the type identifier and the type specification
    if (type_id != nullptr && type_spec != nullptr) {
      if (type_spec->getIdentifier() == nullptr) {
        type_spec->setIdentifier(type_id);
      }
      type_id->setTypeSpec(type_spec);
    } else {
      token = synchronize(TypeDefinitionsParser::followSet());
    }
    token = currentToken();
    if (token->type() == PascalTokenTypeImpl::SEMICOLON) {
      while (token->type() == PascalTokenTypeImpl::SEMICOLON) {
        token = nextToken();
      }
    } else if (TypeDefinitionsParser::nextStartSet().contains(token->type())) {
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    }
    token = synchronize(TypeDefinitionsParser::identifierSet());
  }
  return nullptr;
}

PascalSubparserTopDownBase::TokenTypeSet TypeDefinitionsParser::identifierSet() {
  auto s = DeclarationsParser::varStartSet();
  s.insert(PascalTokenTypeImpl::IDENTIFIER);
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet TypeDefinitionsParser::equalsSet() {
  auto s = ConstantDefinitionsParser::constantStartSet();
  s.insert({
    PascalTokenTypeImpl::EQUALS,
    PascalTokenTypeImpl::SEMICOLON
  });
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet TypeDefinitionsParser::followSet() {
  TokenTypeSet s{PascalTokenTypeImpl::SEMICOLON};
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet TypeDefinitionsParser::nextStartSet() {
  auto s = DeclarationsParser::varStartSet();
  s.insert({
    PascalTokenTypeImpl::SEMICOLON,
    PascalTokenTypeImpl::IDENTIFIER
  });
  return s;
}
