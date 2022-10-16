#include "DeclaredRoutineParser.h"
#include "BlockParser.h"
#include <fmt/format.h>

DeclaredRoutineParser::DeclaredRoutineParser(const std::shared_ptr<PascalParserTopDown> &parent)
    : PascalSubparserTopDownBase(parent), mDummyCounter(0) {

}

DeclaredRoutineParser::~DeclaredRoutineParser() {

}

std::shared_ptr<ICodeNodeImplBase> DeclaredRoutineParser::parse(
    std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  auto routine_defn = DefinitionImpl::UNDEFINED;
  std::string dummy_name;
  std::shared_ptr<SymbolTableEntryImplBase> routine_id = nullptr;
  auto routine_type = token->type();
  // initialize
  switch (routine_type) {
    using enum PascalTokenTypeImpl;
    case PROGRAM: {
      token = nextToken(); // consume PROGRAM
      routine_defn = DefinitionImpl::PROGRAM;
      dummy_name = boost::algorithm::to_lower_copy(std::string{"DummyProgramName"});
      break;
    }
    case PROCEDURE: {
      token = nextToken();
      routine_defn = DefinitionImpl::PROCEDURE;
      dummy_name = boost::algorithm::to_lower_copy(std::string{"DummyProcedureName_"}) +
          fmt::format("{:03d}", ++mDummyCounter);
      break;
    }
    case FUNCTION: {
      token = nextToken();
      routine_defn = DefinitionImpl::FUNCTION;
      dummy_name = boost::algorithm::to_lower_copy(std::string{"DummyFunctionName_"}) +
                   fmt::format("{:03d}", ++mDummyCounter);
      break;
    }
    default: {
      routine_defn = DefinitionImpl::PROGRAM;
      dummy_name = boost::algorithm::to_lower_copy(std::string{"DummyProgramName"});
      break;
    }
  }
  routine_id = parseRoutineName(token, dummy_name);
  routine_id->setDefinition(routine_defn);
  token = currentToken();
  routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>(
      std::vector<std::shared_ptr<SymbolTableEntryImplBase>>{});
  // push the routine's new symbol table onto the stack
  if (routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>() == RoutineCodeImpl::FORWARD) {
    auto symbol_table = routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB>();
    getSymbolTableStack()->push(symbol_table);
  } else {
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB>(getSymbolTableStack()->push());
  }
  // program: set the program identifier in the symbol table stack
  if (routine_defn == DefinitionImpl::PROGRAM) {
    getSymbolTableStack()->setProgramId(routine_id);
  } else if (routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>() != RoutineCodeImpl::FORWARD) {
    // non-forwarded procedure of function: append to the parent's list of routines
    auto subroutines = parent_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>();
    subroutines.push_back(routine_id);
    // TODO: do I need to set it back?
    parent_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>(subroutines);
  }
  // if the routine was forwarded, there should not be any formal parameters or a function return type.
  // but parse them anyway if they are there
  if (routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>() == RoutineCodeImpl::FORWARD) {
    if (token->type() != PascalTokenTypeImpl::SEMICOLON) {
      errorHandler()->flag(token, PascalErrorCode::ALREADY_FORWARDED, currentParser());
      parseHeader(token, routine_id);
    }
  } else {
    // parse the routine's formal parameters and function return type
    parseHeader(token, routine_id);
  }
  // look for the semicolon
  token = currentToken();
  if (token->type() == PascalTokenTypeImpl::SEMICOLON) {
    do {
      token = nextToken();
    } while (token->type() == PascalTokenTypeImpl::SEMICOLON);
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
  }
  // parse the routine's block or forward declaration
  // create new intermediate code for the routine
  auto intermediate_code = std::shared_ptr<ICodeImplBase>(createICode());
  if ((token->type() == PascalTokenTypeImpl::IDENTIFIER) &&
      (boost::algorithm::to_lower_copy(token->text()) == "forward")) {
    // consume "forward"
    token = nextToken();
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>(RoutineCodeImpl::FORWARD);
  } else {
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>(RoutineCodeImpl::DECLARED);
    BlockParser block_parser(currentParser());
    auto root_node = block_parser.parse(token, routine_id);
    intermediate_code->setRoot(root_node);
  }
  routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>(intermediate_code);
  getSymbolTableStack()->pop();
  // TODO: the book returns routine_id
  // return routine_id;
  return intermediate_code->getRoot();
}

std::shared_ptr<SymbolTableEntryImplBase>
DeclaredRoutineParser::parseRoutineName(std::shared_ptr<PascalToken> token, const std::string &dummy_name) {
  std::shared_ptr<SymbolTableEntryImplBase> routine_id = nullptr;
  // parse the routine name identifier
  if (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    auto routine_name = boost::algorithm::to_lower_copy(token->text());
    // TODO
  }
  return std::shared_ptr<SymbolTableEntryImplBase>();
}
