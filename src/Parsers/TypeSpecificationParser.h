#ifndef TYPESPECIFICATIONPARSER_H
#define TYPESPECIFICATIONPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class TypeSpecificationParser : public PascalSubparserTopDownBase
{
public:
  explicit TypeSpecificationParser(PascalParserTopDown& parent);
  virtual ~TypeSpecificationParser();
  std::unique_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // TYPESPECIFICATIONPARSER_H
