#ifndef CONSTANTDEFINITIONSPARSER_H
#define CONSTANTDEFINITIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class ConstantDefinitionsParser : public PascalSubparserTopDownBase
{
public:
  explicit ConstantDefinitionsParser(PascalParserTopDown& parent);
  virtual ~ConstantDefinitionsParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // CONSTANTDEFINITIONSPARSER_H
