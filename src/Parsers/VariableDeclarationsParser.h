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
  explicit VariableDeclarationsParser(PascalParserTopDown& parent);
  virtual ~VariableDeclarationsParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
  virtual void parseIdentifierSublist(std::shared_ptr<PascalToken> token);
  virtual void setDefinition(const DefinitionImpl& definition);
private:
  DefinitionImpl mDefinition;
};

#endif // VARIABLEDECLARATIONSPARSER_H
