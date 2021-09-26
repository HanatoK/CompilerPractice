#ifndef ASSIGNMENTSTATEMENTPARSER_H
#define ASSIGNMENTSTATEMENTPARSER_H

#include <set>

#include "PascalFrontend.h"

class AssignmentStatementParser : public PascalSubparserTopDownBase
{
public:
  explicit AssignmentStatementParser(PascalParserTopDown& parent);
  virtual ~AssignmentStatementParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
  static const std::set<PascalTokenTypeImpl> mColonEqualsSet;
};

#endif // ASSIGNMENTSTATEMENTPARSER_H
