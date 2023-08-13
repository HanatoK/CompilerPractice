#include "ProgramParser.h"
#include "DeclarationsParser.h"
#include "DeclaredRoutineParser.h"

ProgramParser::ProgramParser(const std::shared_ptr<PascalParserTopDown> &parent) : PascalSubparserTopDownBase(parent) {

}

PascalSubparserTopDownBase::TokenTypeSet ProgramParser::programStartSet() {
  PascalSubparserTopDownBase::TokenTypeSet s{
      PascalTokenTypeImpl::PROGRAM,     PascalTokenTypeImpl::SEMICOLON
  };
  s.merge(DeclarationsParser::declarationStartSet());
  return s;
}

ProgramParser::~ProgramParser() {

}

std::shared_ptr<ICodeNodeImplBase> ProgramParser::parse(
    std::shared_ptr<PascalToken> token,
    std::shared_ptr<SymbolTableEntryImplBase> parentId) {
  token = synchronize(ProgramParser::programStartSet());
  // parse the program
  DeclaredRoutineParser routine_parser = DeclaredRoutineParser(currentParser());
  routine_parser.parse(token, parentId);
  // look for the final period
  token = currentToken();
  if (token->type() != PascalTokenTypeImpl::DOT) {
    errorHandler()->flag(token, PascalErrorCode::MISSING_PERIOD, currentParser());
  }
  return nullptr;
}
