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

#endif // UTILITIES_H
