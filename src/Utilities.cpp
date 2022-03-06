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

void CrossReferencer::printRecords(const std::vector<std::shared_ptr<TypeSpecImplBase> >& records) const
{
  for (const auto& record_type: records) {
    const auto record_id = record_type->getIdentifier();
    const auto name = record_id != nullptr ? record_id->name() : std::string{"<unnamed>"};
    fmt::print("\n--- RECORD {} ---", name);
    printColumnHeadings();
    // print the entries in the record's symbol table
    const auto symbol_table = std::any_cast<std::shared_ptr<SymbolTableImplBase>>(record_type->getAttribute(TypeKeyImpl::RECORD_SYMTAB));
    std::vector<std::shared_ptr<TypeSpecImplBase>> new_record_types;
    printSymbolTable(symbol_table, new_record_types);
    // print cross-reference tables for any nested records
    if (new_record_types.size() > 0) {
      printRecords(new_record_types);
    }
  }
}

void CrossReferencer::printEntry(const std::shared_ptr<const SymbolTableEntryImplBase>& entry,
                                 std::vector<std::shared_ptr<TypeSpecImplBase> >& record_types) const
{
  const auto definition = entry->getDefinition();
  const auto nesting_level = entry->symbolTable()->nestingLevel();
  fmt::print("{}Defined as: {}\n", INDENT, definitionimpl_to_string(definition));
  fmt::print("{}Scope nesting level: {}\n", INDENT, std::to_string(nesting_level));
  // print the type specification
  const auto type_spec = entry->getTypeSpec();
  switch (definition) {
    case DefinitionImpl::CONSTANT: {
      const auto val = entry->getAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE);
      fmt::print("{}Value = {}\n", INDENT, any_to_string(val));
      // print the type details only if the type is unnamed
      if (type_spec->getIdentifier() == nullptr) {
        printTypeDetail(type_spec, record_types);
      }
      break;
    }
    case DefinitionImpl::ENUMERATION_CONSTANT: {
      const auto val = entry->getAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE);
      fmt::print("{}Value = {}\n", INDENT, any_to_string(val));
      break;
    }
    case DefinitionImpl::TYPE: {
      // print the type details only when the type is defined at the first time
      if (entry.get() == type_spec->getIdentifier()) {
        printTypeDetail(type_spec, record_types);
      }
      break;
    }
    case DefinitionImpl::VARIABLE: {
      // print the type details only if the type is unnamed
      if (type_spec->getIdentifier() == nullptr) {
        printTypeDetail(type_spec, record_types);
      }
      break;
    }
    default: {
      fmt::print("BUG: {}\n", definitionimpl_to_string(definition));
      break;
    }
  }
}

void CrossReferencer::printType(const std::shared_ptr<TypeSpecImplBase>& type_spec) const
{
  if (type_spec != nullptr) {
    const auto form_str = typeformimpl_to_string(type_spec->form());
    const auto type_id = type_spec->getIdentifier();
    const auto type_name = (type_id != nullptr) ? type_id->name() : "<unnamed>";
    fmt::print("{}Type form = {}, Type id = {}\n", INDENT, form_str, type_name);
  }
}

void CrossReferencer::printTypeDetail(const std::shared_ptr<TypeSpecImplBase>& type_spec,
                                      std::vector<std::shared_ptr<TypeSpecImplBase> >& record_types) const
{
  const auto form = type_spec->form();
  switch (form) {
    case TypeFormImpl::ENUMERATION: {
      const auto constant_ids_any = type_spec->getAttribute(TypeKeyImpl::ENUMERATION_CONSTANTS);
      if (constant_ids_any.has_value()) {
        fmt::print("{}--- Enumeration constants ---\n", INDENT);
        const auto constant_ids = std::any_cast<std::vector<std::shared_ptr<SymbolTableEntryImplBase>>>(constant_ids_any);
        for (const auto& id: constant_ids) {
          const auto name = id->name();
          const auto val = id->getAttribute(SymbolTableKeyTypeImpl::CONSTANT_VALUE);
          fmt::print("{}{: >{}} = {}\n", INDENT, name, NAME_WIDTH, any_to_string(val));
        }
      } else {
        fmt::print("BUG: CrossReferencer::printTypeDetail empty attribute: {}\n",
                   "TypeKeyImpl::ENUMERATION_CONSTANTS");
      }
      break;
    }
    case TypeFormImpl::SUBRANGE: {
      const auto min_val = type_spec->getAttribute(TypeKeyImpl::SUBRANGE_MIN_VALUE);
      const auto max_val = type_spec->getAttribute(TypeKeyImpl::SUBRANGE_MAX_VALUE);
      const auto base_type_spec_any = type_spec->getAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE);
      fmt::print("{}--- Base type ---\n", INDENT);
      if (base_type_spec_any.has_value()) {
        const auto base_type_spec = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(base_type_spec_any);
        printType(base_type_spec);
        // print the base type details only if the type is unnamed
        if (base_type_spec->getIdentifier() == nullptr) {
          printTypeDetail(base_type_spec, record_types);
        }
        fmt::print("{}Range = {}..{}\n", INDENT, any_to_string(min_val), any_to_string(max_val));
      } else {
        fmt::print("BUG: CrossReferencer::printTypeDetail empty attribute: {}\n",
                   "TypeKeyImpl::SUBRANGE_BASE_TYPE");
      }
      break;
    }
    case TypeFormImpl::ARRAY: {
      const auto index_type_any = type_spec->getAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE);
      const auto element_type_any = type_spec->getAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE);
      const auto count_any = type_spec->getAttribute(TypeKeyImpl::ARRAY_ELEMENT_COUNT);
      fmt::print("{}--- INDEX TYPE ---\n", INDENT);
      if (index_type_any.has_value()) {
        const auto index_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(index_type_any);
        printType(index_type);
        if (index_type->getIdentifier() == nullptr) {
          printTypeDetail(index_type, record_types);
        }
        fmt::print("{}--- ELEMENT TYPE---\n", INDENT);
        if (element_type_any.has_value() && count_any.has_value()) {
          const auto element_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(element_type_any);
          const auto count = std::any_cast<PascalInteger>(count_any);
          printType(element_type);
          fmt::print("{}{} elements\n", INDENT, count);
          // print the element type details only if the type is unnamed
          if (element_type->getIdentifier() == nullptr) {
            printTypeDetail(element_type, record_types);
          }
        }
      }
      break;
    }
    case TypeFormImpl::RECORD: {
      record_types.push_back(type_spec);
      break;
    }
    case TypeFormImpl::SCALAR: {
      fmt::print("scalar type\n");
      break;
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
