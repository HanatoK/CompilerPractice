#ifndef CONSTANTDEFINITIONSPARSER_H
#define CONSTANTDEFINITIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class ConstantDefinitionsParser : public PascalSubparserTopDownBase
{
public:
  enum class SignType {NEGATIVE, POSITIVE, NOSIGN};
  static TokenTypeSet identifierSet();
  static TokenTypeSet constantStartSet();
  static TokenTypeSet equalsSet();
  static TokenTypeSet nextStartSet();
  explicit ConstantDefinitionsParser(const std::shared_ptr<PascalParserTopDown>& parent);
  ~ConstantDefinitionsParser() override;
  std::shared_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
  virtual VariableValueT parseConstant(std::shared_ptr<PascalToken>& token);
  virtual VariableValueT parseIdentifierConstant(std::shared_ptr<PascalToken>& token, SignType sign);
  virtual std::shared_ptr<TypeSpecImplBase> getConstantType(const VariableValueT& value) const;
  virtual std::shared_ptr<TypeSpecImplBase> getConstantType(const std::shared_ptr<PascalToken>& token);
};

#endif // CONSTANTDEFINITIONSPARSER_H
