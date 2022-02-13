#ifndef CONSTANTDEFINITIONSPARSER_H
#define CONSTANTDEFINITIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class ConstantDefinitionsParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet identifierSet();
  static TokenTypeSet constantStartSet();
  static TokenTypeSet equalsSet();
  static TokenTypeSet nextStartSet();
  explicit ConstantDefinitionsParser(PascalParserTopDown& parent);
  virtual ~ConstantDefinitionsParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
  virtual std::any parseConstant(std::shared_ptr<PascalToken>& token);
  virtual std::any parseIdentifierConstant(std::shared_ptr<PascalToken>& token, int sign);
  virtual std::shared_ptr<TypeSpecImplBase> getConstantType(const std::any& value) const;
  virtual std::shared_ptr<TypeSpecImplBase> getConstantType(std::shared_ptr<PascalToken>& token);
};

#endif // CONSTANTDEFINITIONSPARSER_H
