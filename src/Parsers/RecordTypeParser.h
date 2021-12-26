#ifndef RECORDTYPEPARSER_H
#define RECORDTYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class RecordTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit RecordTypeParser(PascalParserTopDown& parent);
  virtual ~RecordTypeParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
};

#endif // RECORDTYPEPARSER_H
