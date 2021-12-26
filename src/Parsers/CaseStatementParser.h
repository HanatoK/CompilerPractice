#ifndef CASESTATEMENTPARSER_H
#define CASESTATEMENTPARSER_H

#include "PascalFrontend.h"

class CaseStatementParser : public PascalSubparserTopDownBase {
public:
  explicit CaseStatementParser(PascalParserTopDown &parent);
  virtual std::unique_ptr<ICodeNodeImplBase>
  parse(std::shared_ptr<PascalToken> token);
private:
  virtual std::unique_ptr<ICodeNodeImplBase>
  parseBranch(std::shared_ptr<PascalToken> token,
              std::vector<std::any> &constant_set);
  virtual void parseConstantList(
      std::shared_ptr<PascalToken> token,
      std::unique_ptr<ICodeNodeImplBase>
          &constants_node,
      std::vector<std::any> &constant_set);
  virtual std::unique_ptr<ICodeNodeImplBase>
  parseConstant(std::shared_ptr<PascalToken> token,
                std::vector<std::any> &constant_set);
  virtual std::unique_ptr<ICodeNodeImplBase> parseIdentifierConstant(std::shared_ptr<PascalToken> token,
                          const PascalTokenTypeImpl sign);
  virtual std::unique_ptr<ICodeNodeImplBase>
  parseIntegerConstant(const std::string& value, const PascalTokenTypeImpl sign);
  virtual std::unique_ptr<ICodeNodeImplBase>
  parseCharacterConstant(std::shared_ptr<PascalToken> token,
                         const std::string& value, const PascalTokenTypeImpl sign);
  PascalInteger getNegateNodeValue(const std::unique_ptr<ICodeNodeImplBase>& node);
};

#endif // CASESTATEMENTPARSER_H
