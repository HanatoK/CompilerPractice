#include "DeclarationsParser.h"
#include "ConstantDefinitionsParser.h"
#include "TypeDefinitionsParser.h"
#include "VariableDeclarationsParser.h"

DeclarationsParser::DeclarationsParser(PascalParserTopDown& parent)
  : PascalSubparserTopDownBase(parent) {}

DeclarationsParser::~DeclarationsParser()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > DeclarationsParser::parse(std::shared_ptr<PascalToken> token)
{
  token = synchronize(mDeclarationStartSet);
  if (token->type() == PascalTokenTypeImpl::CONST) {
    token = nextToken();
    ConstantDefinitionsParser constant_definition_parser(*currentParser());
    constant_definition_parser.parse(token);
  }
  token = synchronize(mTypeStartSet);
  if (token->type() == PascalTokenTypeImpl::TYPE) {
    token = nextToken();
    TypeDefinitionsParser type_definitions_parser(*currentParser());
    type_definitions_parser.parse(token);
  }
  token = synchronize(mVarStartSet);
  if (token->type() == PascalTokenTypeImpl::VAR) {
    token = nextToken();
    VariableDeclarationsParser vairable_declarations_parser(*currentParser());
    vairable_declarations_parser.setDefinition(VARIABLE); // TODO
    vairable_declarations_parser.parse(token);
  }
  token = synchronize(mRoutineStartSet);
}
