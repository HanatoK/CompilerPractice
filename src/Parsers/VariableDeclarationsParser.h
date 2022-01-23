#ifndef VARIABLEDECLARATIONSPARSER_H
#define VARIABLEDECLARATIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class VariableDeclarationsParser : public PascalSubparserTopDownBase
{
public:
  explicit VariableDeclarationsParser(PascalParserTopDown& parent);
  virtual ~VariableDeclarationsParser();
  virtual std::unique_ptr<ICodeNodeImplBase> parse(std::shared_ptr<PascalToken> token);
private:
  DefinitionImpl mDefinition;
};

#endif // VARIABLEDECLARATIONSPARSER_H
