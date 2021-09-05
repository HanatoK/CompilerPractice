#include "Utilities.h"
#include "IntermediateImpl.h"

#include <string>
#include <fmt/format.h>

void CrossReferencer::print(const std::shared_ptr<SymbolTableStack<SymbolTableKeyTypeImpl> > symbol_table_stack) {
  fmt::print("{:=^{}}\n", "CROSS-REFERENCE TABLE", NAME_WIDTH * 4);
  printColumnHeadings();
  printSymbolTable(symbol_table_stack->localSymbolTable());
}

void CrossReferencer::printColumnHeadings() {
  fmt::print("{: >{}}{: >{}}\n", "Identifier", NAME_WIDTH, "Line numbers", NAME_WIDTH * 3);
  fmt::print("{:->{}}\n", "", NAME_WIDTH * 4);
}

void CrossReferencer::printSymbolTable(
    const std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>> symbol_table) {
  auto sorted_list = symbol_table->sortedEntries();
  for (const auto &elem : sorted_list) {
    const auto line_numbers = elem->lineNumbers();
    fmt::print("{: >{}}", elem->name(), NAME_WIDTH);
    std::string numbers;
    for (const auto &line_number : line_numbers) {
      numbers += fmt::format("{:0>3d} ", line_number);
    }
    fmt::print("{: >{}}\n", numbers, NAME_WIDTH * 3);
  }
}

ParseTreePrinter::ParseTreePrinter(std::ostream &os):
  mOutputStream(os), mLength(0), mLineIndentation(""), mLine("")
{
  mIndentSpaces = "";
  for (int i = 0; i < INDENT_WIDTH; ++i) {
    mIndentSpaces += " ";
  }
}

void ParseTreePrinter::print(const std::shared_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &intermediate_code)
{
  mOutputStream << fmt::format("{:=^{}}\n", "CROSS-REFERENCE TABLE", LINE_WIDTH);
  printNode(intermediate_code->getRoot());
  printLine();
}

void ParseTreePrinter::printNode(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  // opening tag
  appendOutputLine(mLineIndentation);
  appendOutputLine("<" + node->toString());
//  auto child_nodes = node->children();
  if (node->numChildren() > 0) {
    appendOutputLine(">");
    printLine();
    printChildNodes(node);
    appendOutputLine(mLineIndentation);
    appendOutputLine("</" + node->toString() + ">");
  } else {
    appendOutputLine("");
    appendOutputLine("/>");
  }
  printLine();
}

void ParseTreePrinter::printAttributes(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{
  const auto saved_indentation = mLineIndentation;
  mLineIndentation += mIndentSpaces;
//  const auto& attribute_table = node->attributeTable();
  for (auto it = node->attributeMapBegin(); it != node->attributeMapEnd(); ++it) {
    std::string key_string;
    switch (it->first) {
      case ICodeKeyTypeImpl::LINE: {
        key_string += "LINE";
        break;
      }
      case ICodeKeyTypeImpl::ID: {
        key_string += "ID";
        break;
      }
      case ICodeKeyTypeImpl::VALUE: {
        key_string += "VALUE";
        break;
      }
    }
    printAttributes(key_string, it->second);
  }
  mLineIndentation = saved_indentation;
}

void ParseTreePrinter::printAttributes(const std::string &key, const std::any &value)
{
  // if the value is a symbol table entry, use the identifier's name
  // else just use the value string
  std::string value_string;
  if (value.type() == typeid(std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>)) {
    const auto tmp_value = std::any_cast<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>(value);
    value_string = tmp_value->name();
  } else {
    value_string = any_to_string(value);
  }
  const std::string text = boost::algorithm::to_lower_copy(key) + "=\"" + value_string + "\"";
  appendOutputLine("");
  appendOutputLine(text);
  if (value.type() == typeid(std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>)) {
    const auto tmp_value = std::any_cast<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>(value);
    const int level = tmp_value->symbolTable()->nestingLevel();
    printAttributes("LEVEL", level);
  }
}

void ParseTreePrinter::printChildNodes(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> >  &parent_node)
{
  auto saved_indentation = mLineIndentation;
  mLineIndentation += mIndentSpaces;
//  for (const auto& elem: parent_node) {
//    printNode(elem);
//  }
  for (auto it = parent_node->childrenBegin(); it != parent_node->childrenEnd(); ++it) {
    printNode(*it);
  }
  mLineIndentation = saved_indentation;
}

void ParseTreePrinter::printTypeSpec(const std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> > &node)
{

}

void ParseTreePrinter::appendOutputLine(const std::string &text)
{
  const int text_length = text.size();
  bool line_break = false;
  if (mLength + text_length > LINE_WIDTH) {
    printLine();
    mLine += mLineIndentation;
    mLength = mLineIndentation.length();
    line_break = true;
  }
  if (!(line_break && (text == " "))) {
    mLine += text;
    mLength += text_length;
  }
}

void ParseTreePrinter::printLine()
{
  if (mLength > 0) {
    mOutputStream << mLine;
    mLine.clear();
    mLength = 0;
  }
}
