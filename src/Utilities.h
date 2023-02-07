#ifndef UTILITIES_H
#define UTILITIES_H

#include "Intermediate.h"
#include "Common.h"

class CrossReferencer {
private:
  static constexpr int NAME_WIDTH = 16;
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
  static constexpr int INDENT_WIDTH = 4;
  static constexpr int LINE_WIDTH = 80;
  explicit ParseTreePrinter(std::ostream& os);
  void print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  void printNode(const std::shared_ptr<const ICodeNodeImplBase> &node);
  void printAttributes(const std::shared_ptr<const ICodeNodeImplBase>& node);
  void printAttributes(const std::string& key, const std::shared_ptr<SymbolTableEntryImplBase>& value);
  void printAttributes(const std::string& key, const VariableValueT& value);
  void printAttributes(const std::string& key, const int& value);
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
private:
  static std::string foldString(const std::string& s, int fold_size = 16, const std::string& sep = "\\n");
public:
  explicit ParseTreePrinterDot(std::ostream& os);
  void print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  std::string printNode(const std::shared_ptr<const ICodeNodeImplBase>& node);
};

#endif // UTILITIES_H
