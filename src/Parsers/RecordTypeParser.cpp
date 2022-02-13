#include "RecordTypeParser.h"
#include "VariableDeclarationsParser.h"
#include "DeclarationsParser.h"

RecordTypeParser::RecordTypeParser(PascalParserTopDown& parent): PascalSubparserTopDownBase(parent)
{

}

RecordTypeParser::~RecordTypeParser()
{

}

std::shared_ptr<TypeSpecImplBase> RecordTypeParser::parseSpec(std::shared_ptr<PascalToken> token)
{
  // TODO
  auto record_type = createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(TypeFormImpl::RECORD);
  token = nextToken();
  // push a new symbol table for the record type specification
  record_type->setAttribute(TypeKeyImpl::RECORD_SYMTAB, getSymbolTableStack()->push());
  // parse the field declarations
  VariableDeclarationsParser parser(*currentParser());
  parser.setDefinition(DefinitionImpl::FIELD);
  parser.parse(token);
  // pop off the record symbol table
  getSymbolTableStack()->pop();
  // synchronize at the END
  token = synchronize(RecordTypeParser::recordEndSet());
  // look for the END
  if (token->type() == PascalTokenTypeImpl::END) {
    // consume NED
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_END, currentParser());
  }
  return record_type;
}

PascalSubparserTopDownBase::TokenTypeSet RecordTypeParser::recordEndSet() {
  auto s = DeclarationsParser::varStartSet();
  s.insert({
    PascalTokenTypeImpl::END,
    PascalTokenTypeImpl::SEMICOLON
  });
  return s;
}
