#include "IntermediateImpl.h"

SymbolTableStackImpl::SymbolTableStackImpl(): SymbolTableStack()
{
  mCurrentNestingLevel = 0;
  mStack.append(std::move(createSymbolTable(mCurrentNestingLevel)));
}

int SymbolTableStackImpl::currentNestingLevel() const
{
  return mCurrentNestingLevel;
}

std::shared_ptr<SymbolTable> SymbolTableStackImpl::localSymbolTable() const
{
  return mStack[mCurrentNestingLevel];
}

std::shared_ptr<SymbolTableEntry> SymbolTableStackImpl::enterLocal(const QString &name)
{
  return mStack[mCurrentNestingLevel]->enter(name);
}

std::shared_ptr<SymbolTableEntry> SymbolTableStackImpl::lookupLocal(const QString &name)
{
  return mStack[mCurrentNestingLevel]->lookup(name);
}

std::shared_ptr<SymbolTableEntry> SymbolTableStackImpl::lookup(const QString &name)
{
  return lookupLocal(name);
}

std::unique_ptr<SymbolTableEntry> createSymbolTableEntry(
  const QString &name, SymbolTable *symbolTable)
{
  return std::make_unique<SymbolTableEntryImpl>(name, symbolTable);
}

std::unique_ptr<SymbolTable> createSymbolTable(int nestingLevel)
{
  return std::make_unique<SymbolTableImpl>(nestingLevel);
}

std::unique_ptr<SymbolTableStack> createSymbolTableStack()
{
  return std::make_unique<SymbolTableStackImpl>();
}

SymbolTableImpl::SymbolTableImpl(int nesting_level): SymbolTable(nesting_level)
{
  mNestingLevel = nesting_level;
}

int SymbolTableImpl::nestingLevel() const
{
  return mNestingLevel;
}

std::shared_ptr<SymbolTableEntry> SymbolTableImpl::lookup(const QString &name)
{
  auto search = mSymbolMap.find(name);
  if (search != mSymbolMap.end()) {
    return search->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SymbolTableEntry> SymbolTableImpl::enter(const QString &name)
{
  mSymbolMap[name] = createSymbolTableEntry(name, this);
  return mSymbolMap[name];
}

QList<std::shared_ptr<SymbolTableEntry>> SymbolTableImpl::sortedEntries()
{
  // std::map is already sorted
  QList<std::shared_ptr<SymbolTableEntry>> result;
  for (auto it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it) {
    result.push_back(it->second);
  }
  return result;
}

SymbolTableEntryImpl::SymbolTableEntryImpl(
  const QString &name, SymbolTable *symbol_table):
  SymbolTableEntry(name, symbol_table)
{
  mName = name;
  mSymbolTable = symbol_table;
}

QString SymbolTableEntryImpl::name() const
{
  return mName;
}

SymbolTable *SymbolTableEntryImpl::symbolTable() const
{
  return mSymbolTable;
}

void SymbolTableEntryImpl::appendLineNumber(int line_number)
{
  mLineNumbers.append(line_number);
}

QList<int> SymbolTableEntryImpl::lineNumbers() const
{
  return mLineNumbers;
}

void SymbolTableEntryImpl::setAttribute(const SymbolTableKey *key, const QVariant &value)
{
  mEntryMap[dynamic_cast<const SymbolTableKeyImpl*>(key)->type()] = value;
}

QVariant SymbolTableEntryImpl::getAttribute(const SymbolTableKey *key, bool *ok)
{
  auto search = mEntryMap.find(dynamic_cast<const SymbolTableKeyImpl*>(key)->type());
  if (search != mEntryMap.end()) {
    if (ok) {
      *ok = true;
    }
    return search->second;
  } else {
    if (ok) {
      *ok = false;
    }
    return QVariant();
  }
}

int SymbolTableKeyImpl::type() const
{
  return int(mType);
}

ICodeImpl::ICodeImpl(): ICode()
{

}

std::shared_ptr<ICodeNode> ICodeImpl::setRoot(std::shared_ptr<ICodeNode> node)
{
  mRoot = node;
  return getRoot();
}

std::shared_ptr<ICodeNode> ICodeImpl::getRoot() const
{
  return mRoot;
}

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeType &type): ICodeNode()
{
  mType = type;
  mParent = nullptr;
}

const ICodeNode *&ICodeNodeImpl::parent()
{
  return mParent;
}

ICodeNodeType ICodeNodeImpl::type() const
{
  return mType;
}

std::shared_ptr<ICodeNode> ICodeNodeImpl::addChild(std::shared_ptr<ICodeNode> node)
{
  if (node != nullptr) {
    mChildren.push_back(node);
    node->parent() = dynamic_cast<const ICodeNode*>(this);
  }
  return node;
}

std::vector<std::shared_ptr<ICodeNode> > ICodeNodeImpl::children() const
{
  return mChildren;
}

void ICodeNodeImpl::setAttribute(const ICodeKey &key, const QVariant &value)
{
  mHashTable[key] = value;
}

QVariant ICodeNodeImpl::getAttribute(const ICodeKey &key) const
{
  const auto search = mHashTable.find(key);
  if (search != mHashTable.end()) {
    return search->second;
  } else {
    return QVariant();
  }
}

std::unique_ptr<ICodeNode> ICodeNodeImpl::clone() const
{
  return std::make_unique<ICodeNodeImpl>(*this);
}
