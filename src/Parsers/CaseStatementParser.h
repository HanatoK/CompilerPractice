#ifndef CASESTATEMENTPARSER_H
#define CASESTATEMENTPARSER_H

#include "PascalFrontend.h"

class CaseStatementParser : public PascalSubparserTopDownBase {
public:
  explicit CaseStatementParser(PascalParserTopDown &parent);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parse(std::shared_ptr<PascalToken> token);
private:
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parseBranch(std::shared_ptr<PascalToken> token,
              std::vector<std::any> &constant_set);
  virtual void parseConstantList(
      std::shared_ptr<PascalToken> token,
      std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
          &constants_node,
      std::vector<std::any> &constant_set);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parseConstant(std::shared_ptr<PascalToken> token,
                std::vector<std::any> &constant_set);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parseIdentifierConstant(std::shared_ptr<PascalToken> token,
                          const PascalTokenTypeImpl sign);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parseIntegerConstant(const std::string& value, const PascalTokenTypeImpl sign);
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  parseCharacterConstant(std::shared_ptr<PascalToken> token,
                         const std::string& value, const PascalTokenTypeImpl sign);
};

#endif // CASESTATEMENTPARSER_H
