#ifndef TYPEDEFINITIONSPARSER_H
#define TYPEDEFINITIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class TypeDefinitionsParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet identifierSet();
  static TokenTypeSet equalsSet();
  static TokenTypeSet followSet();
  static TokenTypeSet nextStartSet();
  explicit TypeDefinitionsParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~TypeDefinitionsParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token) override;
};

#endif // TYPEDEFINITIONSPARSER_H
