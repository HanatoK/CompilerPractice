#ifndef VARIABLEPARSER_H
#define VARIABLEPARSER_H

#include "PascalFrontend.h"


class VariableParser: public PascalSubparserTopDownBase {
public:
  static TokenTypeSet subscriptFieldStartSet();
  static TokenTypeSet rightBracketSet();
  explicit VariableParser(const std::shared_ptr<PascalParserTopDown>& parent);
  ~VariableParser() override;
  std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
  std::shared_ptr<ICodeNodeImplBase> parseVariable(std::shared_ptr<PascalToken> token,
                                                   std::shared_ptr<SymbolTableEntryImplBase> id);
private:
  std::shared_ptr<ICodeNodeImplBase> parseSubscripts(std::shared_ptr<TypeSpecImplBase> variable_type);
  std::shared_ptr<ICodeNodeImplBase> parseField(std::shared_ptr<TypeSpecImplBase> variable_type);
};


#endif // VARIABLEPARSER_H
