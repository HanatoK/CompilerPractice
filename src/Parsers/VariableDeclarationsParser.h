#ifndef VARIABLEDECLARATIONSPARSER_H
#define VARIABLEDECLARATIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

/* Pascal vairable declaration example
 * ===================================
 * var
 * weekdays, holidays : days;
 * choice: yes;
 * student_name, emp_name : name;
 * capital: city;
 * cost: expenses;
 * ===================================
 */
class VariableDeclarationsParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet identifierSet();
  static TokenTypeSet nextStartSet();
  static TokenTypeSet identiferStartSet();
  static TokenTypeSet identiferFollowSet();
  static TokenTypeSet commaSet();
  static TokenTypeSet colonSet();
  explicit VariableDeclarationsParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~VariableDeclarationsParser();
  virtual std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token, std::shared_ptr<SymbolTableEntryImplBase> parent_id) override;
  virtual std::vector<std::shared_ptr<SymbolTableEntryImplBase>> parseIdentifierSublist(std::shared_ptr<PascalToken> token);
  virtual void setDefinition(const DefinitionImpl& definition);
  virtual std::shared_ptr<SymbolTableEntryImplBase> parseIdentifier(std::shared_ptr<PascalToken> token);
  virtual std::shared_ptr<TypeSpecImplBase> parseTypeSpec(std::shared_ptr<PascalToken> token);
private:
  DefinitionImpl mDefinition;
};

#endif // VARIABLEDECLARATIONSPARSER_H
