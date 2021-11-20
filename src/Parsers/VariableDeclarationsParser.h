#ifndef VARIABLEDECLARATIONSPARSER_H
#define VARIABLEDECLARATIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class VariableDeclarationsParser : public PascalSubparserTopDownBase
{
public:
  explicit VariableDeclarationsParser(PascalParserTopDown& parent);
  virtual ~VariableDeclarationsParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // VARIABLEDECLARATIONSPARSER_H
