#ifndef SUBRANGETYPEPARSER_H
#define SUBRANGETYPEPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

// Pascal subrange type:
// =================================================================
// type
//   MonthInt = 1 .. 12;
//   Capital = 'A' .. 'Z';
//   ControlChar = ^A .. ^Z;  { `^A' = `Chr (1)' is a BP extension }
// begin
// end.

class SubrangeTypeParser : public PascalSubparserTopDownBase
{
public:
  explicit SubrangeTypeParser(const std::shared_ptr<PascalParserTopDown>& parent);
  ~SubrangeTypeParser() override;
  std::unique_ptr<TypeSpecImplBase> parseSpec(std::shared_ptr<PascalToken> token);
  VariableValueT checkValueType(const std::shared_ptr<PascalToken>& token,
                                const VariableValueT& value,
                                const std::shared_ptr<TypeSpecImplBase>& type);
};

#endif // SUBRANGETYPEPARSER_H
