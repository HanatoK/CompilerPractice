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
//  const SymbolTableKeyImpl* key_impl = static_cast<const SymbolTableKeyImpl*>(key);
  mEntryMap[*(static_cast<const SymbolTableKeyImpl*>(key))] = value;
}

QVariant SymbolTableEntryImpl::getAttribute(const SymbolTableKey *key, bool *ok)
{
//  const SymbolTableKeyImpl* key_impl = static_cast<const SymbolTableKeyImpl*>(key);
  auto search = mEntryMap.find(*(static_cast<const SymbolTableKeyImpl*>(key)));
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

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeType *pType): ICodeNode()
{
  const auto node_type_impl = static_cast<const ICodeNodeTypeImpl*>(pType);
  mType = *(node_type_impl);
  mParent = nullptr;
}

const ICodeNode *&ICodeNodeImpl::parent()
{
  return mParent;
}

std::unique_ptr<ICodeNodeType> ICodeNodeImpl::type() const
{
  return std::make_unique<ICodeNodeTypeImpl>(mType);
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

void ICodeNodeImpl::setAttribute(const ICodeKey *key, const QVariant &value)
{
  const ICodeKeyImpl* key_impl = static_cast<const ICodeKeyImpl*>(key);
  mHashTable[*(key_impl)] = value;
}

QVariant ICodeNodeImpl::getAttribute(const ICodeKey *key) const
{
  const ICodeKeyImpl* key_impl = static_cast<const ICodeKeyImpl*>(key);
  const auto search = mHashTable.find(*(key_impl));
  if (search != mHashTable.end()) {
    return search->second;
  } else {
    return QVariant();
  }
}

std::unique_ptr<ICodeNode> ICodeNodeImpl::copy() const
{
  // only copy this node itself, not the parent and children!
  auto new_node = createICodeNode(&this->mType);
  ICodeNodeImpl* ptr = dynamic_cast<ICodeNodeImpl*>(new_node.get());
  for (auto it = mHashTable.begin(); it != mHashTable.end(); ++it) {
    ptr->mHashTable[it->first] = it->second;
  }
  return std::move(new_node);
}

QString ICodeNodeImpl::toString() const
{
  switch (mType.mType) {
    case ICodeNodeTypeImpl::PROGRAM: return QString("PROGRAM");
    case ICodeNodeTypeImpl::PROCEDURE: return QString("PROCEDURE");
    case ICodeNodeTypeImpl::FUNCTION: return QString("FUNCTION");
    case ICodeNodeTypeImpl::COMPOUND: return QString("COMPOUND");
    case ICodeNodeTypeImpl::ASSIGN: return QString("ASSIGN");
    case ICodeNodeTypeImpl::LOOP: return QString("LOOP");
    case ICodeNodeTypeImpl::TEST: return QString("TEST");
    case ICodeNodeTypeImpl::CALL: return QString("CALL");
    case ICodeNodeTypeImpl::PARAMETERS: return QString("PARAMETERS");
    case ICodeNodeTypeImpl::IF: return QString("IF");
    case ICodeNodeTypeImpl::SELECT: return QString("SELECT");
    case ICodeNodeTypeImpl::SELECT_BRANCH: return QString("SELECT_BRANCH");
    case ICodeNodeTypeImpl::SELECT_CONSTANTS: return QString("SELECT_CONSTANTS");
    case ICodeNodeTypeImpl::NO_OP: return QString("NO_OP");
    case ICodeNodeTypeImpl::EQ: return QString("EQ");
    case ICodeNodeTypeImpl::NE: return QString("NE");
    case ICodeNodeTypeImpl::LT: return QString("LT");
    case ICodeNodeTypeImpl::LE: return QString("LE");
    case ICodeNodeTypeImpl::GT: return QString("GT");
    case ICodeNodeTypeImpl::GE: return QString("GE");
    case ICodeNodeTypeImpl::NOT: return QString("NOT");
    case ICodeNodeTypeImpl::ADD: return QString("ADD");
    case ICodeNodeTypeImpl::SUBTRACT: return QString("SUBTRACT");
    case ICodeNodeTypeImpl::OR: return QString("OR");
    case ICodeNodeTypeImpl::NEGATE: return QString("NEGATE");
    case ICodeNodeTypeImpl::MULTIPLY: return QString("MULTIPLY");
    case ICodeNodeTypeImpl::INTEGER_DIVIDE: return QString("INTEGER_DIVIDE");
    case ICodeNodeTypeImpl::FLOAT_DIVIDE: return QString("FLOAT_DIVIDE");
    case ICodeNodeTypeImpl::MOD: return QString("MOD");
    case ICodeNodeTypeImpl::AND: return QString("AND");
    case ICodeNodeTypeImpl::VARIABLE: return QString("VARIABLE");
    case ICodeNodeTypeImpl::SUBSCRIPTS: return QString("SUBSCRIPTS");
    case ICodeNodeTypeImpl::FIELD: return QString("FIELD");
    case ICodeNodeTypeImpl::INTEGER_CONSTANT: return QString("INTEGER_CONSTANT");
    case ICodeNodeTypeImpl::REAL_CONSTANT: return QString("REAL_CONSTANT");
    case ICodeNodeTypeImpl::STRING_CONSTANT: return QString("STRING_CONSTANT");
    case ICodeNodeTypeImpl::BOOLEAN_CONSTANT: return QString("BOOLEAN_CONSTANT");
  }
}

bool ICodeKeyImpl::operator==(const ICodeKeyImpl &rhs) const
{
  return (this->mType == rhs.mType);
}

bool ICodeKeyImpl::operator!=(const ICodeKeyImpl &rhs) const
{
  return (this->mType != rhs.mType);
}

bool SymbolTableKeyImpl::operator==(const SymbolTableKeyImpl &rhs) const
{
  return (this->mType == rhs.mType);
}

bool SymbolTableKeyImpl::operator!=(const SymbolTableKeyImpl &rhs) const
{
  return (this->mType != rhs.mType);
}

bool ICodeNodeTypeImpl::operator==(const ICodeNodeTypeImpl &rhs) const
{
  return (this->mType == rhs.mType);
}

bool ICodeNodeTypeImpl::operator!=(const ICodeNodeTypeImpl &rhs) const
{
  return (this->mType != rhs.mType);
}

std::unique_ptr<ICode> createICode() {
  return std::make_unique<ICodeImpl>();
}

std::unique_ptr<ICodeNode> createICodeNode(const ICodeNodeType* type) {
  return std::make_unique<ICodeNodeImpl>(type);
}
