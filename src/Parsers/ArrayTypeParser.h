#ifndef ARRAYTYPEPARSER_H
#define ARRAYTYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class ArrayTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit ArrayTypeParser(PascalParserTopDown& parent);
  virtual ~ArrayTypeParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // ARRAYTYPEPARSER_H
