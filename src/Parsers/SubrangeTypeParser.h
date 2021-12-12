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
};

#endif // SUBRANGETYPEPARSER_H
