#ifndef TYPESPECIFICATIONPARSER_H
#define TYPESPECIFICATIONPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class TypeSpecificationParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet typeStartSet();
  explicit TypeSpecificationParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~TypeSpecificationParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // TYPESPECIFICATIONPARSER_H
