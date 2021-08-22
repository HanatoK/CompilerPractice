#include "IntermediateImpl.h"

#include <QDebug>

SymbolTableStackImpl::SymbolTableStackImpl(): SymbolTableStack()
{
  mCurrentNestingLevel = 0;
  mStack.append(createSymbolTable<SymbolTableKeyTypeImpl>(mCurrentNestingLevel));
}

SymbolTableStackImpl::~SymbolTableStackImpl()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

int SymbolTableStackImpl::currentNestingLevel() const
{
  return mCurrentNestingLevel;
}

std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::localSymbolTable() const
{
  return mStack[mCurrentNestingLevel];
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::enterLocal(const QString &name)
{
  return mStack[mCurrentNestingLevel]->enter(name);
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::lookupLocal(const QString &name)
{
  return mStack[mCurrentNestingLevel]->lookup(name);
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::lookup(const QString &name)
{
  return lookupLocal(name);
}

SymbolTableImpl::SymbolTableImpl(int nesting_level): SymbolTable(nesting_level)
{
  mNestingLevel = nesting_level;
}

SymbolTableImpl::~SymbolTableImpl()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

int SymbolTableImpl::nestingLevel() const
{
  return mNestingLevel;
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl> > SymbolTableImpl::lookup(const QString &name)
{
  auto search = mSymbolMap.find(name);
  if (search != mSymbolMap.end()) {
    return search->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableImpl::enter(const QString &name)
{
  mSymbolMap[name] = createSymbolTableEntry(name, this);
  return mSymbolMap[name];
}

QList<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl> > > SymbolTableImpl::sortedEntries()
{
  // std::map is already sorted
  QList<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>> result;
  for (auto it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it) {
    result.push_back(it->second);
  }
  return result;
}

SymbolTableEntryImpl::SymbolTableEntryImpl(const QString &name, SymbolTable<SymbolTableKeyTypeImpl> *symbol_table):
  SymbolTableEntry(name, symbol_table)
{
  mName = name;
  mSymbolTable = symbol_table;
}

SymbolTableEntryImpl::~SymbolTableEntryImpl()
{
//#ifdef DEBUG_DESTRUCTOR
//  qDebug() << "Destructor: " << Q_FUNC_INFO;
//#endif
}

QString SymbolTableEntryImpl::name() const
{
  return mName;
}

SymbolTable<SymbolTableKeyTypeImpl> *SymbolTableEntryImpl::symbolTable() const
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

void SymbolTableEntryImpl::setAttribute(const SymbolTableKeyTypeImpl& key, const QVariant &value)
{
//  const SymbolTableKeyImpl* key_impl = static_cast<const SymbolTableKeyImpl*>(key);
  mEntryMap[key] = value;
}

QVariant SymbolTableEntryImpl::getAttribute(const SymbolTableKeyTypeImpl &key, bool *ok)
{
//  const SymbolTableKeyImpl* key_impl = static_cast<const SymbolTableKeyImpl*>(key);
  auto search = mEntryMap.find(key);
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

ICodeImpl::~ICodeImpl()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> ICodeImpl::setRoot(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node)
{
  mRoot = node;
  return getRoot();
}

std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> ICodeImpl::getRoot() const
{
  return mRoot;
}

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeTypeImpl &pType): ICodeNode()
{
  mType = pType;
  mParent = nullptr;
}

ICodeNodeImpl::~ICodeNodeImpl()
{
#ifdef DEBUG_DESTRUCTOR
  qDebug() << "Destructor: " << Q_FUNC_INFO;
#endif
}

const ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> *&ICodeNodeImpl::parent()
{
  return mParent;
}

ICodeNodeTypeImpl ICodeNodeImpl::type() const
{
  return mType;
}

std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> ICodeNodeImpl::addChild(std::shared_ptr<ICodeNode> node)
{
  if (node != nullptr) {
    mChildren.push_back(node);
    node->parent() = dynamic_cast<const ICodeNode*>(this);
  }
  return node;
}

std::vector<std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> > ICodeNodeImpl::children() const
{
  return mChildren;
}

void ICodeNodeImpl::setAttribute(const ICodeKeyTypeImpl& key, const QVariant &value)
{
  mHashTable[key] = value;
}

QVariant ICodeNodeImpl::getAttribute(const ICodeKeyTypeImpl& key) const
{
  const auto search = mHashTable.find(key);
  if (search != mHashTable.end()) {
    return search->second;
  } else {
    return QVariant();
  }
}

std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> ICodeNodeImpl::copy() const
{
  // only copy this node itself, not the parent and children!
  auto new_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(this->mType);
  ICodeNodeImpl* ptr = dynamic_cast<ICodeNodeImpl*>(new_node.get());
  for (auto it = mHashTable.begin(); it != mHashTable.end(); ++it) {
    ptr->mHashTable[it->first] = it->second;
  }
  return std::move(new_node);
}

QString ICodeNodeImpl::toString() const
{
  switch (mType) {
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
    default: return QString("");
  }
}

template <>
std::unique_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> createICode() {
  return std::make_unique<ICodeImpl>();
}

template <>
std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> createICodeNode(const ICodeNodeTypeImpl& type) {
  return std::make_unique<ICodeNodeImpl>(type);
}

template <>
std::unique_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> createSymbolTableEntry(
  const QString &name, SymbolTable<SymbolTableKeyTypeImpl> *symbolTable)
{
  return std::make_unique<SymbolTableEntryImpl>(name, symbolTable);
}

template <>
std::unique_ptr<SymbolTable<SymbolTableKeyTypeImpl>> createSymbolTable(int nestingLevel)
{
  return std::make_unique<SymbolTableImpl>(nestingLevel);
}

template <>
std::unique_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>> createSymbolTableStack()
{
  return std::make_unique<SymbolTableStackImpl>();
}
