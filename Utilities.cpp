#include "Utilities.h"

#include <QTextStream>
#include <QStringLiteral>

void CrossReferencer::print(const std::shared_ptr<SymbolTableStack> symbol_table_stack)
{
  QTextStream out(stdout);
  out << "==========CROSS-REFERENCE TABLE==========\n";
  printColumnHeadings();
  printSymbolTable(symbol_table_stack->localSymbolTable());
}

void CrossReferencer::printColumnHeadings()
{
  QTextStream out(stdout);
  out << QStringLiteral("%1%2\n").arg("Identifier", NAME_WIDTH).arg("Line numbers", NAME_WIDTH*3);
  out << QString("-").repeated(NAME_WIDTH*4) << "\n";
}

void CrossReferencer::printSymbolTable(const std::shared_ptr<SymbolTable> symbol_table)
{
  QTextStream out(stdout);
  auto sorted_list = symbol_table->sortedEntries();
  for (const auto& elem: sorted_list) {
    const auto line_numbers = elem->lineNumbers();
    out << QStringLiteral("%1").arg(elem->name(), NAME_WIDTH);
    QString numbers;
    for (const auto& line_number: line_numbers) {
      numbers += QStringLiteral("%1").arg(line_number, 3, 10, QLatin1Char('0')) + " ";
    }
    out << QString(" %1\n").arg(numbers, NAME_WIDTH*3);
  }
}
