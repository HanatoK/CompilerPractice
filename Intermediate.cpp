#include "Intermediate.h"

SymbolTableStack::SymbolTableStack()
{

}

SymbolTable::SymbolTable(int nestingLevel)
{
  Q_UNUSED(nestingLevel);
}

SymbolTableEntry::SymbolTableEntry(const QString &name, SymbolTable *symbol_table)
{
  Q_UNUSED(name);
  Q_UNUSED(symbol_table);
}

ICode::ICode()
{

}

ICodeNode::ICodeNode()
{

}

