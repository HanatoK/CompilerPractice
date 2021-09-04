#ifndef UTILITIES_H
#define UTILITIES_H

#include "Intermediate.h"
#include "Common.h"

namespace CrossReferencer {

const int NAME_WIDTH = 16;

void print(const std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> symbol_table_stack);

void printColumnHeadings();

void printSymbolTable(const std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>> symbol_table);

};

class ParseTreePrinter {
private:
  static const int INDENT_WIDTH = 4;
  static const int LINE_WIDTH = 80;
  std::ostream& mOutputStream;
  int mLength;
  std::string mIndentSpaces;
  std::string mLineIndentation;
  std::string mLine;
public:
  ParseTreePrinter(std::ostream& os);
  void print(const std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& intermediate_code);
  void printNode(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void printAttributes(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void printAttributes(const std::string& key, const std::any& value);
  void printChildNodes(const std::vector<std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>>& child_nodes);
  void printTypeSpec(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void appendOutputLine(const std::string& text);
  void printLine();
};

#endif // UTILITIES_H
