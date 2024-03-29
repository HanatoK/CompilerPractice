#ifndef CASESTATEMENTPARSER_H
#define CASESTATEMENTPARSER_H

#include "PascalFrontend.h"

class CaseStatementParser : public PascalSubparserTopDownBase {
public:
  static TokenTypeSet constantStartSet();
  static TokenTypeSet ofSet();
  static TokenTypeSet commaSet();
  explicit CaseStatementParser(const std::shared_ptr<PascalParserTopDown>& parent);
  std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
private:
  virtual std::shared_ptr<ICodeNodeImplBase> parseBranch(std::shared_ptr<PascalToken> token,
              std::vector<VariableValueT> &constant_set, const std::shared_ptr<TypeSpecImplBase>& expression_type);
  virtual void parseConstantList(std::shared_ptr<PascalToken> token,
      std::shared_ptr<ICodeNodeImplBase>& constants_node,
      std::vector<VariableValueT> &constant_set, const std::shared_ptr<TypeSpecImplBase>& expression_type);
  virtual std::shared_ptr<ICodeNodeImplBase> parseConstant(std::shared_ptr<PascalToken> token,
                std::vector<VariableValueT> &constant_set, const std::shared_ptr<TypeSpecImplBase>& expression_type);
  virtual std::shared_ptr<ICodeNodeImplBase> parseIdentifierConstant(const std::shared_ptr<PascalToken>& token,
                          PascalTokenTypeImpl sign);
  virtual std::shared_ptr<ICodeNodeImplBase>
  parseIntegerConstant(const std::string& value, PascalTokenTypeImpl sign);
  virtual std::shared_ptr<ICodeNodeImplBase>
  parseCharacterConstant(const std::shared_ptr<PascalToken>& token,
                         const std::string& value, PascalTokenTypeImpl sign);
  static PascalInteger getNegateNodeValue(const std::shared_ptr<ICodeNodeImplBase>& node);
};

#endif // CASESTATEMENTPARSER_H
