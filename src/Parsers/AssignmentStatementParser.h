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
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // ASSIGNMENTSTATEMENTPARSER_H
