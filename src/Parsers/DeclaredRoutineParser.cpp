#include "DeclaredRoutineParser.h"
#include "DeclarationsParser.h"
#include "VariableDeclarationsParser.h"
#include "BlockParser.h"
#include <fmt/format.h>

DeclaredRoutineParser::DeclaredRoutineParser(const std::shared_ptr<PascalParserTopDown> &parent)
    : PascalSubparserTopDownBase(parent), mRootNode(nullptr), mDummyCounter(0) {

}

DeclaredRoutineParser::~DeclaredRoutineParser() {

}

std::shared_ptr<SymbolTableEntryImplBase> DeclaredRoutineParser::parseToSymbolTableEntry(
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
  const auto current_routine_code = routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>();
  if (current_routine_code && (current_routine_code.value() == RoutineCodeImpl::forward)) {
    auto symbol_table = routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB>();
    if (symbol_table) getSymbolTableStack()->push(symbol_table.value());
    else BUG("empty val");
  } else {
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB>(getSymbolTableStack()->push());
  }
  // program: set the program identifier in the symbol table stack
  if (routine_defn == DefinitionImpl::PROGRAM) {
    getSymbolTableStack()->setProgramId(routine_id);
  } else if (current_routine_code && (current_routine_code.value() != RoutineCodeImpl::forward)) {
    // non-forwarded procedure of function: append to the parent's list of routines
    auto subroutines = parent_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>();
    // set it back?
    if (subroutines) {
      subroutines.value().push_back(routine_id);
      parent_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>(subroutines.value());
    }
  }
  // if the routine was forwarded, there should not be any formal parameters or a function return type.
  // but parse them anyway if they are there
  if (current_routine_code && (current_routine_code.value() == RoutineCodeImpl::forward)) {
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
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>(RoutineCodeImpl::forward);
  } else {
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>(RoutineCodeImpl::declared);
    BlockParser block_parser(currentParser());
    mRootNode = block_parser.parse(token, routine_id);
    intermediate_code->setRoot(mRootNode);
  }
  routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>(intermediate_code);
  getSymbolTableStack()->pop();
  return routine_id;
}

std::shared_ptr<SymbolTableEntryImplBase>
DeclaredRoutineParser::parseRoutineName(std::shared_ptr<PascalToken> token, const std::string &dummy_name) {
  std::shared_ptr<SymbolTableEntryImplBase> routine_id = nullptr;
  // parse the routine name identifier
  if (token->type() == PascalTokenTypeImpl::IDENTIFIER) {
    const auto routine_name = boost::algorithm::to_lower_copy(token->text());
    routine_id = getSymbolTableStack()->lookupLocal(routine_name);
    // not already defined locally: enter into the local symbol table
    if (routine_id == nullptr) {
      routine_id = getSymbolTableStack()->enterLocal(routine_name);
    } else if (routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>() != RoutineCodeImpl::forward) {
      // if already defined, it should be a forward definition.
      // Otherwise, this is a redefined identifier.
      routine_id = nullptr;
      errorHandler()->flag(token, PascalErrorCode::IDENTIFIER_REDEFINED, currentParser());
    }
    // consume routine name identifier
    token = nextToken();
  } else {
    errorHandler()->flag(token, PascalErrorCode::MISSING_IDENTIFIER, currentParser());
  }
  if (routine_id == nullptr) {
    // create a dummy routine name symbol table entry if necessary
    routine_id = getSymbolTableStack()->enterLocal(dummy_name);
  }
  return routine_id;
}

void DeclaredRoutineParser::parseHeader(
  std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> routine_id) {
  // parse the routine's formal parameters
  parseFormalParameters(token, routine_id);
  token = currentToken();
  // if this is a function, parse and set its return type
  if (routine_id->getDefinition() == DefinitionImpl::FUNCTION) {
    VariableDeclarationsParser parser(currentParser());
    parser.setDefinition(DefinitionImpl::FUNCTION);
    auto type = parser.parseTypeSpec(token);
    token = currentToken();
    if (type != nullptr) {
      // Pascal does not allow the reture type to be an array or record
      const auto form = type->form();
      if ((form == TypeFormImpl::ARRAY) || (form == TypeFormImpl::RECORD)) {
        errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
      }
    } else {
      // missing return type
      type = Predefined::instance().undefinedType;
    }
    routine_id->setTypeSpec(type);
    token = currentToken();
  }
}

PascalSubparserTopDownBase::TokenTypeSet DeclaredRoutineParser::parameterSet() {
  auto s = DeclarationsParser::declarationStartSet();
  s.insert({PascalTokenTypeImpl::VAR,
            PascalTokenTypeImpl::IDENTIFIER,
            PascalTokenTypeImpl::RIGHT_PAREN});
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclaredRoutineParser::leftParenSet() {
  auto s = DeclarationsParser::declarationStartSet();
  s.insert({PascalTokenTypeImpl::LEFT_PAREN,
            PascalTokenTypeImpl::SEMICOLON,
            PascalTokenTypeImpl::COLON});
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclaredRoutineParser::rightParenSet() {
  auto s = DeclaredRoutineParser::leftParenSet();
  s.insert(PascalTokenTypeImpl::RIGHT_PAREN);
  s.erase(PascalTokenTypeImpl::LEFT_PAREN);
  return s;
}

void DeclaredRoutineParser::parseFormalParameters(std::shared_ptr<PascalToken> token,
                                                  std::shared_ptr<SymbolTableEntryImplBase> routine_id) {
  // parse the formal parameters if there is an opening left parenthesis
  token = synchronize(DeclaredRoutineParser::leftParenSet());
  if (token->type() == PascalTokenTypeImpl::LEFT_PAREN) {
    token = nextToken(); // consume (
    std::vector<std::shared_ptr<SymbolTableEntryImplBase>> parms;
    token = synchronize(DeclaredRoutineParser::parameterSet());
    auto token_type = token->type();
    // loop to parse sublists of formal parameter declarations
    while ((token_type == PascalTokenTypeImpl::IDENTIFIER) ||
           (token_type == PascalTokenTypeImpl::VAR)) {
      const auto sublist = parseParmSublist(token, routine_id);
      parms.insert(parms.end(), sublist.begin(), sublist.end());
      token = currentToken();
      token_type = token->type();
    }
    // closing right parenthesis
    if (token->type() == PascalTokenTypeImpl::RIGHT_PAREN) {
      token = nextToken(); // consume )
    } else {
      errorHandler()->flag(token, PascalErrorCode::MISSING_RIGHT_PAREN, currentParser());
    }
    routine_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_PARMS>(parms);
  }
}

PascalSubparserTopDownBase::TokenTypeSet DeclaredRoutineParser::parameterFollowSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::COLON,
    PascalTokenTypeImpl::RIGHT_PAREN,
    PascalTokenTypeImpl::SEMICOLON};
  s.merge(DeclarationsParser::declarationStartSet());
  return s;
}

PascalSubparserTopDownBase::TokenTypeSet DeclaredRoutineParser::commaSet() {
  TokenTypeSet s{
    PascalTokenTypeImpl::COMMA,
    PascalTokenTypeImpl::COLON,
    PascalTokenTypeImpl::IDENTIFIER,
    PascalTokenTypeImpl::RIGHT_PAREN,
    PascalTokenTypeImpl::SEMICOLON};
  s.merge(DeclarationsParser::declarationStartSet());
  return s;
}

// listing 11-7
std::vector<std::shared_ptr<SymbolTableEntryImplBase>>
DeclaredRoutineParser::parseParmSublist(std::shared_ptr<PascalToken> token,
                                        std::shared_ptr<SymbolTableEntryImplBase> routine_id) {
  const bool is_program = (routine_id->getDefinition() == DefinitionImpl::PROGRAM);
  DefinitionImpl parm_defn = is_program ? DefinitionImpl::PROGRAM_PARM : DefinitionImpl::UNDEFINED;
  const auto token_type = token->type();
  // VAR or value parameter?
  // VAR is passed by reference!
  if (token_type == PascalTokenTypeImpl::VAR) {
    if (!is_program) {
      parm_defn = DefinitionImpl::VAR_PARM;
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_VAR_PARM, currentParser());
    }
    token = nextToken(); // consume VAR
  } else if (!is_program) {
    parm_defn = DefinitionImpl::VALUE_PARM;
  }
  // should I throw here? No, this is always false.
//  if (parm_defn == DefinitionImpl::UNDEFINED) {
//    throw std::runtime_error("Undefined parameter declaration!");
//  }
  // parse the parameter sub_list and its type specification
  VariableDeclarationsParser parser(currentParser());
  parser.setDefinition(parm_defn);
  auto sub_list = parser.parseIdentifierSublist(token, parameterFollowSet(), commaSet());
  token = currentToken();
//  token_type = token->type();
  // look for one or more semicolons after a sub_list
  if (!is_program) {
    if (token->type() == PascalTokenTypeImpl::SEMICOLON) {
      while (token->type() == PascalTokenTypeImpl::SEMICOLON) {
        token = nextToken();
      }
    } else if (VariableDeclarationsParser::nextStartSet().contains(token->type())) {
      // if at the start of the next sub_list, then missing a semicolon
      errorHandler()->flag(token, PascalErrorCode::MISSING_SEMICOLON, currentParser());
    }
    token = synchronize(DeclaredRoutineParser::parameterSet());
  }
  return sub_list;
}
