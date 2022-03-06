#ifndef DECLARATIONSPARSER_H
#define DECLARATIONSPARSER_H

#include "PascalFrontend.h"
#include "Intermediate.h"

class DeclarationsParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet declarationStartSet();
  static TokenTypeSet typeStartSet();
  static TokenTypeSet varStartSet();
  static TokenTypeSet routineStartSet();
  explicit DeclarationsParser(PascalParserTopDown& parent);
  virtual ~DeclarationsParser();
  virtual std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> parse(std::shared_ptr<PascalToken> token);
};

#endif // DECLARATIONSPARSER_H
