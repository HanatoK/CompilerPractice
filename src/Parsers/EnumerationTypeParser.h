#ifndef ENUMERATIONTYPEPARSER_H
#define ENUMERATIONTYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

// Pascal enumeration type:
// ===============================================
// program enumerationDemo(input, output, stdErr);
// type
//   fruit = (apple, banana, citrus);

class EnumerationTypeParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet enumConstantStartSet();
  static TokenTypeSet enumDefinitionFollowSet();
  explicit EnumerationTypeParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~EnumerationTypeParser();
  std::shared_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
  void parseEnumerationIdentifier(std::shared_ptr<PascalToken>& token, PascalInteger value,
    std::shared_ptr<TypeSpecImplBase> enumeration_type,
    std::vector<std::weak_ptr<SymbolTableEntryImplBase> >& constants);
};

#endif // ENUMERATIONTYPEPARSER_H
