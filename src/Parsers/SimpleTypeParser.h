#ifndef SIMPLETYPEPARSER_H
#define SIMPLETYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class SimpleTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit SimpleTypeParser(PascalParserTopDown& parent);
  virtual ~SimpleTypeParser();
  std::unique_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // SIMPLETYPEPARSER_H
