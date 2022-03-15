#ifndef BACKEND_H
#define BACKEND_H

#include "Intermediate.h"
#include "Common.h"

#include <boost/signals2.hpp>
#include <utility>
#include <memory>
#include <iostream>

class Backend;

enum class RuntimeErrorCode {
  UNINITIALIZED_VALUE,
  VALUE_RANGE,
  INVALID_CASE_EXPRESSION_VALUE,
  DIVISION_BY_ZERO,
  INVALID_STANDARD_FUNCTION_ARGUMENT,
  INVALID_INPUT,
  STACK_OVERFLOW,
  UNIMPLEMENTED_FEATURE
};

std::string runtimeErrorCodeToString(const RuntimeErrorCode &code);

class RuntimeErrorHandler {
public:
  RuntimeErrorHandler();
  void flag(const std::shared_ptr<const ICodeNodeImplBase>& node,
            const RuntimeErrorCode error_code, const std::shared_ptr<Backend>& backend);
  int errorCount() const;
private:
  static const int MAX_ERRORS = 5;
  int mErrorCount;
};

class Backend: public std::enable_shared_from_this<Backend> {
public:
  Backend();
  virtual ~Backend() {
#ifdef DEBUG_DESTRUCTOR
    std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
  }
  virtual void process(std::shared_ptr<const ICodeImplBase> iCode,
                       std::shared_ptr<SymbolTableStackImplBase> symbol_table_stack) = 0;
  virtual std::string getType() const {
    return "unknown";
  }
  boost::signals2::signal<void(const int, const std::string&)> runtimeErrorMessage;
protected:
  std::shared_ptr<const ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mICode;
  std::shared_ptr<SymbolTableStackImplBase> mSymbolTableStack;
};

std::shared_ptr<Backend> createBackend(const std::string& operation);

#endif // BACKEND_H
