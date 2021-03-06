#ifndef SIMPLETYPEPARSER_H
#define SIMPLETYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class SimpleTypeParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet simpleTypeStartSet();
  explicit SimpleTypeParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~SimpleTypeParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // SIMPLETYPEPARSER_H
