#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Backend.h"
#include "Intermediate.h"

class Executor: public Backend {
public:
  Executor();
  virtual ~Executor();
  virtual void process(std::shared_ptr<const ICodeImplBase> iCode,
                       std::shared_ptr<SymbolTableStackImplBase> symbol_table_stack) override;
  virtual std::string getType() const override {
    return "interpreter";
  }
  //  void summary(int executionCount, int runtimeErrors, float elapsedTime);
  boost::signals2::signal<void(const int, const int, const double)> summary;
  boost::signals2::signal<void(const int)> sourceLineMessage;
  boost::signals2::signal<void(const int, const std::string&, const VariableValueT&)> assignmentMessage;
  friend class SubExecutorBase;
private:
  std::shared_ptr<RuntimeErrorHandler> mErrorHandler;
  int mExecutionCount;
};

class SubExecutorBase {
public:
  explicit SubExecutorBase(const std::shared_ptr<Executor>& executor);
  virtual ~SubExecutorBase();
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNodeImplBase>& node) = 0;
  virtual void sendSourceLineMessage(const std::shared_ptr<ICodeNodeImplBase> &node);
  std::shared_ptr<Executor> currentExecutor();
  std::shared_ptr<RuntimeErrorHandler> errorHandler();
  int& executionCount();
private:
  std::shared_ptr<Executor> mExecutor;
};

#endif // INTERPRETER_H
