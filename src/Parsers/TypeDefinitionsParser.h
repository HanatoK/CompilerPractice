#ifndef TYPEDEFINITIONSPARSER_H
#define TYPEDEFINITIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class TypeDefinitionsParser : public PascalSubparserTopDownBase
{
public:
  explicit TypeDefinitionsParser(PascalParserTopDown& parent);
  virtual ~TypeDefinitionsParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
};

#endif // TYPEDEFINITIONSPARSER_H
