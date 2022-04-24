#include "Utilities.h"
#include "Intermediate.h"

#include <fmt/format.h>
#include <string>

CrossReferencer::CrossReferencer():
  NAME_WIDTH(16), INDENT(NAME_WIDTH * 4, ' ')
{

}

void CrossReferencer::print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack) const {
  fmt::print("{:=^{}}\n", "CROSS-REFERENCE TABLE", NAME_WIDTH * 4);
  const auto& program_id = symbol_table_stack->programId();
  printRoutine(program_id);
}

void CrossReferencer::printColumnHeadings() const {
  fmt::print("\n{: >{}}{: >{}}{} {}\n", "Identifier", NAME_WIDTH,
             "Line numbers", NAME_WIDTH * 2,
             std::string(INDENT.size() - NAME_WIDTH * 3, ' '), "Type specification");
  fmt::print("{:->{}}\n", "", NAME_WIDTH * 7);
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
    fmt::print("{: >{}}\n", numbers, NAME_WIDTH * 2);
    printEntry(elem, record_types);
  }
}

void CrossReferencer::printRoutine(const std::shared_ptr<SymbolTableEntryImplBase>& routine_id) const
{
  const auto definition = routine_id->getDefinition();
  fmt::print("\n*** {} {} ***", definitionimpl_to_string(definition), routine_id->name());
  printColumnHeadings();
  // print the entries in the routine's symbol table
  const auto symbol_table = routine_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB>();
  std::vector<std::shared_ptr<TypeSpecImplBase>> new_record_types;
  printSymbolTable(symbol_table, new_record_types);
  // print cross-reference tables for any records defined in the routine
  if (new_record_types.size() > 0) {
    printRecords(new_record_types);
  }
  // print any procedures and functions defined in the routine
  const auto routine_ids_any = routine_id->getAttribute(SymbolTableKeyTypeImpl::ROUTINE_ROUTINES);
  if (routine_ids_any.has_value()) {
    const auto routine_ids = cast_by_enum<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES>(routine_ids_any);
    for (const auto& i : routine_ids) {
      printRoutine(i);
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
    const auto symbol_table = record_type->getAttribute<TypeKeyImpl::RECORD_SYMTAB>();
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
  fmt::print("{} {}\n", INDENT, "Defined as: " + definitionimpl_to_string(definition));
  fmt::print("{} {}\n", INDENT, "Scope nesting level: " + std::to_string(nesting_level));
  // print the type specification
  const auto type_spec = entry->getTypeSpec();
  printType(type_spec);
  switch (definition) {
    case DefinitionImpl::CONSTANT: {
      const auto val = entry->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
      fmt::print("{} {}\n", INDENT, "Value = " + variable_value_to_string(val));
      // print the type details only if the type is unnamed
      if (type_spec->getIdentifier() == nullptr) {
        printTypeDetail(type_spec, record_types);
      }
      break;
    }
    case DefinitionImpl::ENUMERATION_CONSTANT: {
      const auto val = entry->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
      fmt::print("{} {}\n", INDENT, "Value = " + variable_value_to_string(val));
      break;
    }
    case DefinitionImpl::TYPE: {
      // print the type details only when the type is defined at the first time
      if (entry == type_spec->getIdentifier()) {
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
//    default: {
//      fmt::print("{} {}\n", INDENT, "BUG: " + definitionimpl_to_string(definition));
//      break;
//    }
  }
}

void CrossReferencer::printType(const std::shared_ptr<TypeSpecImplBase>& type_spec) const
{
  if (type_spec != nullptr) {
    const auto form_str = typeformimpl_to_string(type_spec->form());
    const auto type_id = type_spec->getIdentifier();
    const auto type_name = (type_id != nullptr) ? type_id->name() : "<unnamed>";
    fmt::print("{} {}\n", INDENT, "Type form = " + form_str + ", Type id = " + type_name);
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
        fmt::print("{} {}\n", INDENT, "--- Enumeration constants ---");
        const auto constant_ids = std::any_cast<std::vector<std::weak_ptr<SymbolTableEntryImplBase>>>(constant_ids_any);
        for (const auto& id: constant_ids) {
          const auto id_ptr = id.lock();
          const auto name = id_ptr->name();
          const auto val = id_ptr->getAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>();
          fmt::print("{} {}\n", INDENT, name + " = " + variable_value_to_string(val));
        }
      } else {
        fmt::print("BUG: CrossReferencer::printTypeDetail empty attribute: {}\n",
                   "TypeKeyImpl::ENUMERATION_CONSTANTS");
      }
      break;
    }
    case TypeFormImpl::SUBRANGE: {
      const auto min_val = type_spec->getAttribute<TypeKeyImpl::SUBRANGE_MIN_VALUE>();
      const auto max_val = type_spec->getAttribute<TypeKeyImpl::SUBRANGE_MAX_VALUE>();
      const auto base_type_spec_any = type_spec->getAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE);
      fmt::print("{} {}\n", INDENT, "--- Base type ---");
      if (base_type_spec_any.has_value()) {
        const auto base_type_spec = cast_by_enum<TypeKeyImpl::SUBRANGE_BASE_TYPE>(base_type_spec_any);
        printType(base_type_spec);
        // print the base type details only if the type is unnamed
        if (base_type_spec->getIdentifier() == nullptr) {
          printTypeDetail(base_type_spec, record_types);
        }
        fmt::print("{} {}\n", INDENT, "Range: " + variable_value_to_string(min_val) + ".." + variable_value_to_string(max_val));
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
      fmt::print("{} {}\n", INDENT, "--- INDEX TYPE ---");
      if (index_type_any.has_value()) {
        const auto index_type = cast_by_enum<TypeKeyImpl::ARRAY_INDEX_TYPE>(index_type_any);
        printType(index_type);
        if (index_type->getIdentifier() == nullptr) {
          printTypeDetail(index_type, record_types);
        }
        fmt::print("{} {}\n", INDENT, "--- ELEMENT TYPE---");
        if (element_type_any.has_value() && count_any.has_value()) {
          const auto element_type = cast_by_enum<TypeKeyImpl::ARRAY_ELEMENT_TYPE>(element_type_any);
          const auto count = cast_by_enum<TypeKeyImpl::ARRAY_ELEMENT_COUNT>(count_any);
          printType(element_type);
          fmt::print("{} {}\n", INDENT, std::to_string(count) + " elements");
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
    : mOutputStream(os), mLength(0), mIndentSpaces(INDENT_WIDTH, ' ') {
}

void ParseTreePrinter::print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack) {
  const auto tmp_line_width = LINE_WIDTH;
  mOutputStream << fmt::format("{:=^{}}\n", "ParseTreePrinter", tmp_line_width);
  const auto& program_id = symbol_table_stack->programId();
  const auto& iCode = program_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>();
  const auto root_node = iCode->getRoot();
  printNode(root_node);
  printLine();
  mOutputStream << fmt::format("{:=^{}}\n", "", tmp_line_width);
}

void ParseTreePrinter::printNode(const std::shared_ptr<const ICodeNodeImplBase>& node)
{
  if (node != nullptr) {
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
        printAttributes(key_string, cast_by_enum<ICodeKeyTypeImpl::LINE>(it->second));
        break;
      }
      case ICodeKeyTypeImpl::ID: {
        key_string += "ID";
        printAttributes(key_string, cast_by_enum<ICodeKeyTypeImpl::ID>(it->second));
        break;
      }
      case ICodeKeyTypeImpl::VALUE: {
        key_string += "VALUE";
        printAttributes(key_string, cast_by_enum<ICodeKeyTypeImpl::VALUE>(it->second));
        break;
      }
    }
  }
  mLineIndentation = saved_indentation;
}

void ParseTreePrinter::printAttributes(const std::string &key,
                                       const std::shared_ptr<SymbolTableEntryImplBase> &value) {
  // if the value is a symbol table entry, use the identifier's name
  // else just use the value string
  std::string value_string = value->name();
  const std::string text =
      boost::algorithm::to_lower_copy(key) + "=\"" + value_string + "\"";
  appendOutputLine(" ");
  appendOutputLine(text);
  const int level = value->symbolTable()->nestingLevel();
  printAttributes("LEVEL", level);
}

void ParseTreePrinter::printChildNodes(const std::shared_ptr<const ICodeNodeImplBase>& parent_node) {
  const auto saved_indentation = mLineIndentation;
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

void ParseTreePrinter::printTypeSpec(const std::shared_ptr<const ICodeNodeImplBase>& node) {
  const auto type_spec = node->getTypeSpec();
  if (type_spec != nullptr) {
    auto saved_indentation = mLineIndentation;
    mLineIndentation += mIndentSpaces;
    std::string type_name;
    const auto type_id = type_spec->getIdentifier();
    if (type_id != nullptr) {
      // named type: print the type identifier's name
      type_name = type_id->name();
    } else {
      // unnamed type: print an artificial type identifier name
      type_name = "$_anon_" + type_spec->anonymousName() + "$";
    }
    printAttributes("TYPE_ID", type_name);
  }
}

void ParseTreePrinter::appendOutputLine(const std::string &text) {
  // TODO: line wrapping is buggy!
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

void ParseTreePrinter::printAttributes(const std::string &key, const VariableValueT &value) {
  const auto value_string = variable_value_to_string(value);
  const std::string text =
      boost::algorithm::to_lower_copy(key) + "=\"" + value_string + "\"";
  appendOutputLine(" ");
  appendOutputLine(text);
}

void ParseTreePrinter::printAttributes(const std::string &key, const int &value) {
  const auto value_string = std::to_string(value);
  const std::string text =
      boost::algorithm::to_lower_copy(key) + "=\"" + value_string + "\"";
  appendOutputLine(" ");
  appendOutputLine(text);
}

ParseTreePrinterDot::ParseTreePrinterDot(std::ostream &os): mOutputStream(os), mIndex(0)
{

}

void ParseTreePrinterDot::print(const std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack)
{
  const auto& program_id = symbol_table_stack->programId();
  const auto& iCode = program_id->getAttribute<SymbolTableKeyTypeImpl::ROUTINE_ICODE>();
  const auto root_node = iCode->getRoot();
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
        node_label += std::string{"\\n"} + "LINE: " + std::to_string(cast_by_enum<ICodeKeyTypeImpl::LINE>(it->second));
        break;
      }
      case ICodeKeyTypeImpl::ID: {
        node_label += std::string{"\\n"} + "ID: " + cast_by_enum<ICodeKeyTypeImpl::ID>(it->second)->name();
        break;
      }
      case ICodeKeyTypeImpl::VALUE: {
        node_label += std::string{"\\n"} + "VALUE" + ": " + variable_value_to_string(cast_by_enum<ICodeKeyTypeImpl::VALUE>(it->second));
        break;
      }
    }
    const auto type_spec = node->getTypeSpec();
    if (type_spec != nullptr) {
      node_label += std::string{"\\n"} + "TYPE: " + foldString(type_spec->anonymousName());
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

std::string ParseTreePrinterDot::foldString(const std::string& s, int fold_size, const std::string& sep) {
  if (fold_size <= 0) return s;
  std::string result;
  size_t num_folds = (size_t)(s.size() / (size_t)fold_size);
  size_t num_remaining_chars = s.size() % (size_t)fold_size;
  for (size_t i = 0; i < num_folds; ++i) {
    result += s.substr(i * fold_size, fold_size);
    result += sep;
  }
  if (num_remaining_chars > 0) {
    result += s.substr(num_folds * fold_size, num_remaining_chars);
    result += sep;
  }
  // remove the last "\n"
  if (num_folds > 0 || num_remaining_chars > 0) {
    result.erase(result.size() - sep.size(), sep.size());
  }
  return result;
}
