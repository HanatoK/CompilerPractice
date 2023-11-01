#include "DeclarationsParser.h"
#include "ConstantDefinitionsParser.h"
#include "TypeDefinitionsParser.h"
#include "VariableDeclarationsParser.h"
#include "DeclaredRoutineParser.h"

DeclarationsParser::DeclarationsParser(const std::shared_ptr<PascalParserTopDown>& parent)
  : PascalSubparserTopDownBase(parent) {}

DeclarationsParser::~DeclarationsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::shared_ptr<ICodeNodeImplBase> DeclarationsParser::parse(
    std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id)
{
  token = synchronize(declarationStartSet());
  if (token->type() == PascalTokenTypeImpl::CONST) {
    token = nextToken();
    ConstantDefinitionsParser constant_definition_parser(currentParser());
    constant_definition_parser.parse(token, parent_id);
  }
  token = synchronize(typeStartSet());
  if (token->type() == PascalTokenTypeImpl::TYPE) {
    token = nextToken();
    TypeDefinitionsParser type_definitions_parser(currentParser());
    type_definitions_parser.parse(token, parent_id);
  }
  token = synchronize(varStartSet());
  if (token->type() == PascalTokenTypeImpl::VAR) {
    token = nextToken();
    VariableDeclarationsParser vairable_declarations_parser(currentParser());
    vairable_declarations_parser.setDefinition(DefinitionImpl::VARIABLE);
    vairable_declarations_parser.parse(token, parent_id);
  }
  token = synchronize(routineStartSet());
  auto token_type = token->type();
  while ((token_type == PascalTokenTypeImpl::PROCEDURE) ||
         (token_type == PascalTokenTypeImpl::FUNCTION)) {
    DeclaredRoutineParser routine_parser(currentParser());
    routine_parser.parseToSymbolTableEntry(token, parent_id);
    // look for one or more semicolons after a definition
    if (token->type() == PascalTokenTypeImpl::SEMICOLON) {
      while (token->type() == PascalTokenTypeImpl::SEMICOLON) {
        token = nextToken(); // consume the ;
      }
    }
    token = synchronize(routineStartSet());
    token_type = token->type();
  }
  return nullptr;
}

PascalSubparserTopDownBase::TokenTypeSet DeclarationsParser::declarationStartSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
    PascalTokenTypeImpl::CONST,     PascalTokenTypeImpl::TYPE,
    PascalTokenTypeImpl::VAR,       PascalTokenTypeImpl::PROCEDURE,
    PascalTokenTypeImpl::FUNCTION,  PascalTokenTypeImpl::BEGIN
  };
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclarationsParser::typeStartSet() {
  auto s = DeclarationsParser::declarationStartSet();
  s.erase(PascalTokenTypeImpl::CONST);
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclarationsParser::varStartSet() {
  auto s = DeclarationsParser::typeStartSet();
  s.erase(PascalTokenTypeImpl::TYPE);
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclarationsParser::routineStartSet() {
  auto s = DeclarationsParser::varStartSet();
  s.erase(PascalTokenTypeImpl::VAR);
  return s;
}
