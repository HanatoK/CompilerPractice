#include "Utilities.h"
#include "IntermediateImpl.h"

#include <fmt/format.h>
#include <string>

CrossReferencer::CrossReferencer():
  NAME_WIDTH(16), NUMBERS_LABEL(" Line numbers    "),
  NUMBERS_UNDERLINE(" ------------    "), LABEL_WIDTH(NUMBERS_LABEL.size()),
  INDENT_WIDTH(NAME_WIDTH + LABEL_WIDTH), INDENT(INDENT_WIDTH, ' ')
{

}

void CrossReferencer::print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack) const {
  fmt::print("{:=^{}}\n", "CROSS-REFERENCE TABLE", NAME_WIDTH * 4);
  const auto& program_id = symbol_table_stack->programId();
  printRoutine(program_id);
}

void CrossReferencer::printColumnHeadings() const {
  fmt::print("\n{: >{}}{: >{}}{: >{}}", "Identifier", NAME_WIDTH * 3,
             NUMBERS_LABEL, LABEL_WIDTH, "Type specification", NAME_WIDTH * 4);
  fmt::print("{:->{}}{}{:->{}}\n", "", NAME_WIDTH * 3, NUMBERS_UNDERLINE, "", NAME_WIDTH * 4);
}

void CrossReferencer::printSymbolTable(const std::shared_ptr<const SymbolTableImplBase>& symbol_table,
                                       std::vector<std::shared_ptr<TypeSpecImplBase>>& record_types) const {
  auto sorted_list = symbol_table->sortedEntries();
  // loop over the sorted list of symbol table entries
  for (const auto &elem : sorted_list) {
    const auto line_numbers = elem->lineNumbers();
    fmt::print("{: >{}}", elem->name(), NAME_WIDTH);
    std::string numbers;
    for (const auto &line_number : line_numbers) {
      numbers += fmt::format("{:0>3d} ", line_number);
    }
    fmt::print("{: >{}}\n", numbers, NAME_WIDTH * 3);
    printEntry(elem, record_types);
  }
}

void CrossReferencer::printRoutine(const SymbolTableEntryImplBase* const routine_id) const
{
  const auto definition = routine_id->getDefinition();
  fmt::print("\n*** {} {} ***", definitionimpl_to_string(definition), routine_id->name());
  printColumnHeadings();
  // print the entries in the routine's symbol table
  const auto symbol_table = std::any_cast<std::shared_ptr<SymbolTableImplBase>>(routine_id->getAttribute(SymbolTableKeyTypeImpl::ROUTINE_SYMTAB));
  std::vector<std::shared_ptr<TypeSpecImplBase>> new_record_types;
  printSymbolTable(symbol_table, new_record_types);
  // print cross-reference tables for any records defined in the routine
  if (new_record_types.size() > 0) {
    printRecords(new_record_types);
  }
  // print any procedures and functions defined in the routine
  const auto routine_ids_any = routine_id->getAttribute(SymbolTableKeyTypeImpl::ROUTINE_ROUTINES);
  if (routine_ids_any.has_value()) {
    const auto routine_ids = std::any_cast<std::vector<std::shared_ptr<SymbolTableEntryImplBase>>>(routine_ids_any);
    for (const auto& i : routine_ids) {
      printRoutine(i.get());
    }
  }
}

void CrossReferencer::printEntry(const std::shared_ptr<const SymbolTableEntryImplBase>& entry,
                                 std::vector<std::shared_ptr<TypeSpecImplBase> >& records) const
{
  const auto definition = entry->getDefinition();
  const auto nesting_level = entry->symbolTable()->nestingLevel();
  fmt::print("{}\n", INDENT + "Defined as: " + definitionimpl_to_string(definition));
  fmt::print("{}\n", INDENT + "Scope nesting level: " + std::to_string(nesting_level));
  // print the type specification
  const auto type_spec = entry->getTypeSpec();
  // TODO
  switch (definition) {
    case DefinitionImpl::CONSTANT: {

    }
  }
}

ParseTreePrinter::ParseTreePrinter(std::ostream &os)
    : mOutputStream(os), mLength(0) {
  mIndentSpaces = "";
  for (int i = 0; i < INDENT_WIDTH; ++i) {
    mIndentSpaces += " ";
  }
}

void ParseTreePrinter::print(const std::shared_ptr<const ICodeImplBase>& intermediate_code) {
  const auto tmp_line_width = LINE_WIDTH;
  mOutputStream << fmt::format("{:=^{}}\n", "ParseTreePrinter", tmp_line_width);
  printNode(intermediate_code->getRoot());
  printLine();
  mOutputStream << fmt::format("{:=^{}}\n", "", tmp_line_width);
}

void ParseTreePrinter::printNode(
    const std::shared_ptr<ICodeNodeImplBase const>
        &node) {
  // opening tag
  appendOutputLine(mLineIndentation);
  appendOutputLine("<" + node->toString());
  printAttributes(node);
  printTypeSpec(node);
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

void ParseTreePrinter::printAttributes(const std::shared_ptr<const ICodeNodeImplBase>& node) {
  const auto saved_indentation = mLineIndentation;
  mLineIndentation += mIndentSpaces;
  //  const auto& attribute_table = node->attributeTable();
  for (auto it = node->attributeMapBegin(); it != node->attributeMapEnd();
       ++it) {
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

void ParseTreePrinter::printAttributes(const std::string &key,
                                       const std::any &value) {
  // if the value is a symbol table entry, use the identifier's name
  // else just use the value string
  std::string value_string;
  if (value.type() ==
      typeid(std::shared_ptr<SymbolTableEntryImplBase>)) {
    const auto tmp_value = std::any_cast<
        std::shared_ptr<SymbolTableEntryImplBase>>(value);
    value_string = tmp_value->name();
  } else {
    value_string = any_to_string(value);
  }
  const std::string text =
      boost::algorithm::to_lower_copy(key) + "=\"" + value_string + "\"";
  appendOutputLine(" ");
  appendOutputLine(text);
  if (value.type() ==
      typeid(std::shared_ptr<SymbolTableEntryImplBase>)) {
    const auto tmp_value = std::any_cast<
        std::shared_ptr<SymbolTableEntryImplBase>>(value);
    const int level = tmp_value->symbolTable()->nestingLevel();
    printAttributes("LEVEL", level);
  }
}

void ParseTreePrinter::printChildNodes(const std::shared_ptr<const ICodeNodeImplBase>& parent_node) {
  auto saved_indentation = mLineIndentation;
  mLineIndentation += mIndentSpaces;
  //  for (const auto& elem: parent_node) {
  //    printNode(elem);
  //  }
  for (auto it = parent_node->childrenBegin(); it != parent_node->childrenEnd();
       ++it) {
    printNode(*it);
  }
  mLineIndentation = saved_indentation;
}

void ParseTreePrinter::printTypeSpec(
    const std::shared_ptr<const ICodeNodeImplBase>
        &node) {}

void ParseTreePrinter::appendOutputLine(const std::string &text) {
  const auto text_length = text.size();
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

void ParseTreePrinter::printLine() {
  if (mLength > 0) {
    mOutputStream << mLine << '\n';
    mLine.clear();
    mLength = 0;
  }
}

ParseTreePrinterDot::ParseTreePrinterDot(std::ostream &os): mOutputStream(os), mIndex(0)
{

}

void ParseTreePrinterDot::print(const std::shared_ptr<const ICodeImplBase> &intermediate_code)
{
  auto root_node = intermediate_code->getRoot();
  printNode(root_node);
  mOutputStream << "digraph \"parse tree\"\n{\n"
                << "  rankdir=\"LR\"\n"
                << "  {\n";
  mOutputStream << "    node [shape=box, style=\"rounded\"]\n";
  for (size_t i = 0; i < mNodeStyleLines.size(); ++i) {
    mOutputStream << "    " << mNodeStyleLines[i] << "\n";
  }
  mOutputStream << "  }\n";
  for (size_t i = 0; i < mNodeConnectionLines.size(); ++i) {
    mOutputStream << "  " << mNodeConnectionLines[i] << "\n";
  }
  mOutputStream << "}";
}

std::string ParseTreePrinterDot::printNode(const std::shared_ptr<const ICodeNodeImplBase>& node)
{
  // print current node
  std::string node_label = node->toString();
  for (auto it = node->attributeMapBegin(); it != node->attributeMapEnd(); ++it) {
    const auto value = it->second;
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
    if (value.type() ==
        typeid(std::shared_ptr<SymbolTableEntryImplBase>)) {
      const auto tmp_value = std::any_cast<
          std::shared_ptr<SymbolTableEntryImplBase>>(value);

      node_label += "\\n" + key_string + ": " + tmp_value->name();
    } else {
      node_label += "\\n" + key_string + ": " + any_to_string(value);
    }
  }
  std::string node_name = "node" + std::to_string(mIndex++) + " ";
  const std::string node_style_line = node_name + "[label = \"" + node_label + "\"]";
  mNodeStyleLines.push_back(node_style_line);
  if (node->numChildren() > 0) {
    std::string child_nodes;
    for (auto it = node->childrenBegin(); it != node->childrenEnd(); ++it) {
      child_nodes += printNode(*it);
    }
    const std::string node_connection_line = node_name + "-> { " + child_nodes + "}";
    mNodeConnectionLines.push_back(node_connection_line);
  }
  return node_name;
}
