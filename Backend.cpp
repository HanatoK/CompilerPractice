#include "Backend.h"
#include "IntermediateImpl.h"
#include "Interpreter.h"
#include "Compiler.h"

#include <cstdlib>

std::unique_ptr<Backend> createBackend(const std::string &operation) {
  if (boost::iequals(operation, "compile")) {
    return std::make_unique<CodeGenerator>();
  } else if (boost::iequals(operation, "interpret")) {
    return std::make_unique<Executor>();
  } else {
    return nullptr;
  }
}

RuntimeErrorHandler::RuntimeErrorHandler(): mErrorCount(0)
{

}

void RuntimeErrorHandler::flag(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > node,
                               RuntimeErrorCode error_code, Backend *backend)
{
  const ICodeNodeBase* ptr = node.get();
  int line_number = 0;
  do {
    if (ptr->getAttribute(ICodeKeyTypeImpl::LINE).has_value()) {
      line_number = std::any_cast<int>(ptr->getAttribute(ICodeKeyTypeImpl::LINE));
      break;
    }
    ptr = ptr->parent();
  } while (ptr != nullptr);
  backend->runtimeErrorMessage(line_number, runtimeErrorCodeToString(error_code));
  if (++mErrorCount > MAX_ERRORS) {
    std::cerr << "ABORTED AFTER TOO MANY RUNTIME ERRORS.\n";
    std::exit(-1);
  }
}

std::string runtimeErrorCodeToString(RuntimeErrorCode code)
{
  switch (code) {
    case RuntimeErrorCode::DIVISION_BY_ZERO: {return "Division by zero";}
    case RuntimeErrorCode::INVALID_CASE_EXPRESSION_VALUE: {return "Invalid CASE expression value";}
    case RuntimeErrorCode::INVALID_INPUT: {return "Invalid input";}
    case RuntimeErrorCode::INVALID_STANDARD_FUNCTION_ARGUMENT: {return "Invalid standard function argument";}
    case RuntimeErrorCode::STACK_OVERFLOW: {return "Runtime stack overflow";}
    case RuntimeErrorCode::UNIMPLEMENTED_FEATURE: {return "Unimplemented runtime feature";}
    case RuntimeErrorCode::UNINITIALIZED_VALUE: {return "Uninitialized value";}
    case RuntimeErrorCode::VALUE_RANGE: {return "Value out of range";}
  }
  return "Unknown runtime error.";
}

Backend::Backend()
{

}
