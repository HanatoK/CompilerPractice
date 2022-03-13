#ifndef UTILITIES_H
#define UTILITIES_H

#include "Intermediate.h"
#include "IntermediateImpl.h"
#include "Common.h"

class CrossReferencer {
private:
  int NAME_WIDTH;
  std::string INDENT;
public:
  CrossReferencer();
  void print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack) const;
  void printColumnHeadings() const;
  void printSymbolTable(const std::shared_ptr<const SymbolTableImplBase > &symbol_table,
                        std::vector<std::shared_ptr<TypeSpecImplBase> >& record_types) const;
  void printRoutine(const std::shared_ptr<SymbolTableEntryImplBase>& routine_id) const;
  void printRecords(const std::vector<std::shared_ptr<TypeSpecImplBase>>& records) const;
  void printEntry(const std::shared_ptr<SymbolTableEntryImplBase const>& entry,
                  std::vector<std::shared_ptr<TypeSpecImplBase> >& record_types) const;
  void printType(const std::shared_ptr<TypeSpecImplBase>& type_spec) const;
  void printTypeDetail(const std::shared_ptr<TypeSpecImplBase>& type_spec,
                       std::vector<std::shared_ptr<TypeSpecImplBase>>& record_types) const;
};

class ParseTreePrinter {
private:
  std::ostream& mOutputStream;
  size_t mLength;
  std::string mIndentSpaces;
  std::string mLineIndentation;
  std::string mLine;
public:
  static const int INDENT_WIDTH = 4;
  static const int LINE_WIDTH = 80;
  explicit ParseTreePrinter(std::ostream& os);
  void print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  void printNode(const std::shared_ptr<const ICodeNodeImplBase> &node);
  void printAttributes(const std::shared_ptr<const ICodeNodeImplBase>& node);
  void printAttributes(const std::string& key, const std::any& value);
  void printChildNodes(const std::shared_ptr<const ICodeNodeImplBase> &parent_node);
  void printTypeSpec(const std::shared_ptr<const ICodeNodeImplBase>& node);
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
  explicit ParseTreePrinterDot(std::ostream& os);
  void print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  std::string printNode(const std::shared_ptr<const ICodeNodeImplBase>& node);
};

#endif // UTILITIES_H
