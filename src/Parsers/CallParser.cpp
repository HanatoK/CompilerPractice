#include "CallParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"

CallParser::CallParser(const std::shared_ptr <PascalParserTopDown> &parent) : PascalSubparserTopDownBase(parent) {

}

CallParser::~CallParser() {

}

std::shared_ptr<ICodeNodeImplBase>
CallParser::parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  auto pfId = getSymbolTableStack()->lookup(boost::algorithm::to_lower_copy(token->text()));
  auto routine_code = pfId->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>();
  // TODO: I think this is ill-formed
  StatementParser parser =
      (routine_code == RoutineCodeImpl::declared) ||
      (routine_code == RoutineCodeImpl::forward) ?
      CallDeclaredParser(currentParser()):
      CallStandardParser(currentParser());
  return parser.parse(token, parent_id);
}

std::shared_ptr<ICodeNodeImplBase>
CallParser::parseActualParameters(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> pfId,
                                  bool isDeclared, bool isReadReadln, bool isWriteWriteln) {
  auto expressionParser = ExpressionParser(currentParser());
  auto parmsNode = to_shared(createICodeNode(ICodeNodeTypeImpl::PARAMETERS));
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> formalParms;
  int parmCount = 0;
  int parmIndex = -1;
  if (isDeclared) {
    formalParms = pfId->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_PARMS>();
    parmCount = static_cast<int>(formalParms.size());
  }
  if (token->type() != PascalTokenTypeImpl::LEFT_PAREN) {
    if (parmCount != 0) {
      errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
    }
    return nullptr;
  }
  // consume opening (
  token = nextToken();
  // loop to parse each actual parameter
  while (token->type() != PascalTokenTypeImpl::RIGHT_PAREN) {
    auto actualNode = expressionParser.parse(token, nullptr);
    // declared procedure or function: check the number of actual parameters
    // and check each actual parameter against the corresponding formal parameter
    if (isDeclared) {
      if (++parmIndex < parmCount) {
        auto formalId = formalParms[parmIndex];
        checkActualParameter(token, formalId, actualNode);
      } else if (parmIndex == parmCount) {
        errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
      }
    } else if (isReadReadln) {
      // read or readln: each actual parameter must be a variable that is
      // a scalar, boolean, or subrange of integer
      auto type = actualNode->getTypeSpec();
      auto form = type->form();
      // TODO
    }
  }
}

CallDeclaredParser::CallDeclaredParser(const std::shared_ptr<PascalParserTopDown> &parent) : CallParser(
    parent) {

}

CallDeclaredParser::~CallDeclaredParser() {

}

// parse a call to a declared procedure or function
std::shared_ptr<ICodeNodeImplBase>
CallDeclaredParser::parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  // create the CALL node
  auto callNode = to_shared(createICodeNode(ICodeNodeTypeImpl::CALL));
  auto pfId = getSymbolTableStack()->lookup(boost::algorithm::to_lower_copy(token->text()));
  callNode->setAttribute<ICodeKeyTypeImpl::ID>(pfId);
  callNode->setTypeSpec(pfId->getTypeSpec());
  // consume procedure of function identifier
  token = nextToken();
  auto parmsNode = parseActualParameters(token, pfId, true, false, false);
  callNode->addChild(parmsNode);
  return callNode;
}

CallStandardParser::CallStandardParser(const std::shared_ptr<PascalParserTopDown> &parent) : CallParser(
    parent) {

}

CallStandardParser::~CallStandardParser() {

}

// function calls to the standard functions
std::shared_ptr<ICodeNodeImplBase>
CallStandardParser::parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  auto callNode = to_shared(createICodeNode(ICodeNodeTypeImpl::CALL));
  auto pfId = getSymbolTableStack()->lookup(boost::algorithm::to_lower_copy(token->text()));
  auto routineCode = pfId->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>();
  callNode->setAttribute<ICodeKeyTypeImpl::ID>(pfId);
  token = nextToken();
  using enum RoutineCodeImpl;
  switch (routineCode) {
    case read:
    case readln: return parseReadReadln(token, callNode, pfId);
    case write:
    case writeln: return parseWriteWriteln(token, callNode, pfId);
    case eof:
    case eoln: return parseEofEoln(token, callNode, pfId);
    case abs:
    case sqr: return parseAbsSqr(token, callNode, pfId);
    case arctan:
    case cos:
    case exp:
    case ln:
    case sin:
    case sqrt: return parseArctanCosExpLnSinSqrt(token, callNode, pfId);
    case pred:
    case succ: return parsePredSucc(token, callNode, pfId);
    case chr: return parseChr(token, callNode, pfId);
    case odd: return parseOdd(token, callNode, pfId);
    case ord: return parseOrd(token, callNode, pfId);
    case round:
    case trunc: return parseRoundTrunc(token, callNode, pfId);
    default: return nullptr;
  }
}

std::shared_ptr<ICodeNodeImplBase>
CallStandardParser::parseReadReadln(std::shared_ptr<PascalToken> token, std::shared_ptr<ICodeNodeImplBase> callNode,
                                    std::shared_ptr<SymbolTableEntryImplBase> pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, true, false);
  callNode->addChild(parmsNode);
  // read must have parameters
  if ((pfId == Predefined::instance().readId) && (callNode->numChildren() == 0)) {
    errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
  }
  return callNode;
}
