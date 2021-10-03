#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Backend.h"

class Executor: public Backend {
public:
  Executor();
  virtual ~Executor();
  virtual void process(std::shared_ptr<const ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > iCode,
                       std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);
  virtual std::string getType() const {
    return "interpreter";
  }
  //  void summary(int executionCount, int runtimeErrors, float elapsedTime);
  boost::signals2::signal<void(const int, const int, const double)> summary;
  boost::signals2::signal<void(const int)> sourceLineMessage;
  boost::signals2::signal<void(const int, const std::string&, const std::any&)> assignmentMessage;
  friend class SubExecutorBase;
private:
  RuntimeErrorHandler* mErrorHandler;
  int mExecutionCount;
};

class SubExecutorBase {
public:
  explicit SubExecutorBase(Executor& executor);
  virtual ~SubExecutorBase();
  virtual std::shared_ptr<SubExecutorBase> execute(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node) = 0;
  virtual void sendSourceLineMessage(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node);
  Executor* currentExecutor();
  RuntimeErrorHandler* errorHandler();
  int& executionCount();
private:
  Executor& mExecutor;
};

#endif // INTERPRETER_H
