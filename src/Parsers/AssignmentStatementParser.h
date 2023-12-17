#ifndef ASSIGNMENTSTATEMENTPARSER_H
#define ASSIGNMENTSTATEMENTPARSER_H

#include <set>

#include "PascalFrontend.h"

class AssignmentStatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet colonEqualsSet();
  explicit AssignmentStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~AssignmentStatementParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
  std::shared_ptr<ICodeNodeImplBase>
  parseFunctionNameAssignment(std::shared_ptr<PascalToken> token,
                              std::shared_ptr<SymbolTableEntryImplBase> parent_id);
private:
  bool isFunctionTarget;
};

#endif // ASSIGNMENTSTATEMENTPARSER_H
