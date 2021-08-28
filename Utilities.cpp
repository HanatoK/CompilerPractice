#include "Utilities.h"

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
