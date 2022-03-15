#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class StatementParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet statementStartSet();
  static TokenTypeSet statementFollowSet();
  explicit StatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~StatementParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
  virtual void parseList(std::shared_ptr<PascalToken> token,
                         std::shared_ptr<ICodeNodeImplBase>& parent_node,
                         const PascalTokenTypeImpl &terminator, const PascalErrorCode &error_code);
};

#endif // STATEMENTPARSER_H
