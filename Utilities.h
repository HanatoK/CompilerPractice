#ifndef UTILITIES_H
#define UTILITIES_H

#include "Intermediate.h"

namespace CrossReferencer {

const int NAME_WIDTH = 16;

void print(const std::shared_ptr<SymbolTableStack> symbol_table_stack);

void printColumnHeadings();

void printSymbolTable(const std::shared_ptr<SymbolTable> symbol_table);

};

#endif // UTILITIES_H
