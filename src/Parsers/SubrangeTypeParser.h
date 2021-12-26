#ifndef SUBRANGETYPEPARSER_H
#define SUBRANGETYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class SubrangeTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit SubrangeTypeParser(PascalParserTopDown& parent);
  virtual ~SubrangeTypeParser();
  std::unique_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
  std::any checkValueType(const PascalToken& token, const std::any& value, const std::shared_ptr<TypeSpecImplBase>& type);
};

#endif // SUBRANGETYPEPARSER_H
