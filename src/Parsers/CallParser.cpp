#include "CallParser.h"
#include "StatementParser.h"
#include "ExpressionParser.h"
#include "TypeChecker.h"

CallParser::CallParser(const std::shared_ptr <PascalParserTopDown> &parent) : PascalSubparserTopDownBase(parent) {

}

CallParser::~CallParser() {

}

std::shared_ptr<ICodeNodeImplBase>
CallParser::parse(std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) {
  auto pfId = getSymbolTableStack()->lookup(boost::algorithm::to_lower_copy(token->text()));
  auto routine_code = pfId->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>();
  // TODO: I think this is ill-formed
//  StatementParser parser =
//      (routine_code == RoutineCodeImpl::declared) ||
//      (routine_code == RoutineCodeImpl::forward) ?
//      CallDeclaredParser(currentParser()):
//      CallStandardParser(currentParser());
//  return parser.parse(token, parent_id);
  if ((routine_code == RoutineCodeImpl::declared) ||
      (routine_code == RoutineCodeImpl::forward)) {
    auto parser = CallDeclaredParser(currentParser());
    return parser.parse(token, parent_id);
  } else {
    auto parser = CallStandardParser(currentParser());
    return parser.parse(token, parent_id);
  }
}

std::shared_ptr<ICodeNodeImplBase>
CallParser::parseActualParameters(std::shared_ptr<PascalToken> token, const std::shared_ptr<SymbolTableEntryImplBase>& pfId,
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
      if (!((actualNode->type() == ICodeNodeTypeImpl::VARIABLE) &&
            ((form == TypeFormImpl::SCALAR) ||
             (type == Predefined::instance().booleanType) ||
             ((form == TypeFormImpl::SUBRANGE) &&
                 (type->baseType() == Predefined::instance().integerType))))) {
        errorHandler()->flag(token, PascalErrorCode::INVALID_VAR_PARM, currentParser());
      }
    } else if (isWriteWriteln) {
      // create a WRITE_PARM node which adopts the expression node
      auto exprNode = std::move(actualNode);
      actualNode = createICodeNode(ICodeNodeTypeImpl::WRITE_PARM);
      actualNode->addChild(exprNode);
      // TODO
    }
  }
}

void
CallParser::checkActualParameter(const std::shared_ptr<PascalToken>& token, const std::shared_ptr<SymbolTableEntryImplBase>& formalId,
                                 const std::shared_ptr<ICodeNodeImplBase>& node) {
  auto formalDefn = formalId->getDefinition();
  auto formalType = formalId->getTypeSpec();
  auto actualType = node->getTypeSpec();
  // VAR parameter: the actual parameter must be a variable
  // of the same type as the formal parameter
  if (formalDefn == DefinitionImpl::VAR_PARM) {
    if ((node->type() != ICodeNodeTypeImpl::VARIABLE) ||
        (actualType != formalType)) {
      errorHandler()->flag(token, PascalErrorCode::INVALID_VAR_PARM, currentParser());
    }
  } else if (!TypeChecker::TypeCompatibility::areAssignmentCompatible(formalType, actualType)) {
    errorHandler()->flag(token, PascalErrorCode::INCOMPATIBLE_TYPES, currentParser());
  }
}

std::shared_ptr<ICodeNodeImplBase> CallParser::parseWriteSpec(std::shared_ptr<PascalToken> token) {
  if (token->type() == PascalTokenTypeImpl::COLON) {
    token = nextToken();
    auto parser = ExpressionParser(currentParser());
    auto spec_node = parser.parse(token, nullptr);
    if (spec_node->type() == ICodeNodeTypeImpl::INTEGER_CONSTANT) {
      return spec_node;
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_NUMBER, currentParser());
      return nullptr;
    }
  } else {
    return nullptr;
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

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseReadReadln(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, true, false);
  callNode->addChild(parmsNode);
  // read must have parameters
  if ((pfId == Predefined::instance().readId) && (callNode->numChildren() == 0)) {
    errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase>
CallStandardParser::parseWriteWriteln(const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
                                      const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, true);
  callNode->addChild(parmsNode);
  if ((pfId == Predefined::instance().writeId) && (callNode->numChildren() == 0)) {
    errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase>
CallStandardParser::parseEofEoln(const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
                                 const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be no actual parameters
  if (checkParmCount(token, parmsNode, 0)) {
    callNode->setTypeSpec(Predefined::instance().booleanType);
  }
  return callNode;
}

bool CallStandardParser::checkParmCount(
  const std::shared_ptr<PascalToken>& token,
  const std::shared_ptr<ICodeNodeImplBase>& node,
  size_t count) {
  if (((node == nullptr) && (count == 0)) ||
      (node->numChildren() == count)) {
    return true;
  } else {
    errorHandler()->flag(token, PascalErrorCode::WRONG_NUMBER_OF_PARMS, currentParser());
    return false;
  }
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseAbsSqr(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one integer of real parameter
  // the function return type is the same as the parameter type
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().integerType) ||
        (argType == Predefined::instance().realType)) {
      callNode->setTypeSpec(argType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseArctanCosExpLnSinSqrt(
    const std::shared_ptr<PascalToken>& token,
    std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one integer of real parameter
  // the function return type is real
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().integerType) ||
        (argType == Predefined::instance().realType)) {
      callNode->setTypeSpec(Predefined::instance().realType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parsePredSucc(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one integer or enumeration parameter
  // the function return type is the parameter type
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().integerType) ||
        (argType->form() == TypeFormImpl::ENUMERATION)) {
      callNode->setTypeSpec(argType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseChr(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one integer parameter, and return a char type
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().integerType)) {
      callNode->setTypeSpec(Predefined::instance().charType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseOdd(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one integer parameter, and return a boolean
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().integerType)) {
      callNode->setTypeSpec(Predefined::instance().booleanType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase> CallStandardParser::parseOrd(
    const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one char type or enumeration type, and return an int
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().charType) ||
        (argType->form() == TypeFormImpl::ENUMERATION)) {
      callNode->setTypeSpec(Predefined::instance().integerType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}

std::shared_ptr<ICodeNodeImplBase>
CallStandardParser::parseRoundTrunc(const std::shared_ptr<PascalToken>& token, std::shared_ptr<ICodeNodeImplBase> callNode,
                                    const std::shared_ptr<SymbolTableEntryImplBase>& pfId) {
  // parse any actual parameters
  auto parmsNode = parseActualParameters(token, pfId, false, false, false);
  callNode->addChild(parmsNode);
  // there should be one real type, and return an int
  if (checkParmCount(token, parmsNode, 1)) {
    auto argType = (*parmsNode->childrenBegin())->getTypeSpec()->baseType();
    if ((argType == Predefined::instance().realType)) {
      callNode->setTypeSpec(Predefined::instance().integerType);
    } else {
      errorHandler()->flag(token, PascalErrorCode::INVALID_TYPE, currentParser());
    }
  }
  return callNode;
}
