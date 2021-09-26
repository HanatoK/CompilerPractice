#ifndef UTILITIES_H
#define UTILITIES_H

#include "Intermediate.h"
#include "Common.h"

namespace CrossReferencer {

const int NAME_WIDTH = 16;

void print(const std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>& symbol_table_stack);

void printColumnHeadings();

void printSymbolTable(const std::shared_ptr<const SymbolTable<SymbolTableKeyTypeImpl> > &symbol_table);

};

class ParseTreePrinter {
private:
  std::ostream& mOutputStream;
  int mLength;
  std::string mIndentSpaces;
  std::string mLineIndentation;
  std::string mLine;
public:
  static const int INDENT_WIDTH = 4;
  static const int LINE_WIDTH = 80;
  ParseTreePrinter(std::ostream& os);
  void print(const std::shared_ptr<const ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &intermediate_code);
  void printNode(const std::shared_ptr<const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void printAttributes(const std::shared_ptr<const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void printAttributes(const std::string& key, const std::any& value);
  void printChildNodes(const std::shared_ptr<const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &parent_node);
  void printTypeSpec(const std::shared_ptr<const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
  void appendOutputLine(const std::string& text);
  void printLine();
};

class ParseTreePrinterDot {
private:
  std::ostream& mOutputStream;
  int mIndex;
  std::vector<std::string> mNodeStyleLines;
  std::vector<std::string> mNodeConnectionLines;
public:
  ParseTreePrinterDot(std::ostream& os);
  void print(const std::shared_ptr<const ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& intermediate_code);
  std::string printNode(const std::shared_ptr<const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>& node);
};

#endif // UTILITIES_H
