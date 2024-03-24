#ifndef COMPILERPRACTICE_PROGRAMPARSER_H
#define COMPILERPRACTICE_PROGRAMPARSER_H

#include "PascalFrontend.h"


class ProgramParser : public PascalSubparserTopDownBase
{
public:
  static TokenTypeSet programStartSet();
  explicit ProgramParser(const std::shared_ptr<PascalParserTopDown>& parent);
  virtual ~ProgramParser();
  std::shared_ptr<ICodeNodeImplBase> parse(
      std::shared_ptr<PascalToken> token,
      std::shared_ptr<SymbolTableEntryImplBase> parentId);
  auto getRootNode() {return mRootNode;}
private:
  std::shared_ptr<ICodeNodeImplBase> mRootNode;
};


#endif //COMPILERPRACTICE_PROGRAMPARSER_H
