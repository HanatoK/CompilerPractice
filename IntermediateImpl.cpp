#include "IntermediateImpl.h"

#include <iostream>

SymbolTableStackImpl::SymbolTableStackImpl(): SymbolTableStack()
{
  mCurrentNestingLevel = 0;
  mStack.push_back(createSymbolTable<SymbolTableKeyTypeImpl>(mCurrentNestingLevel));
}

SymbolTableStackImpl::~SymbolTableStackImpl()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
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

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::enterLocal(const std::string &name)
{
  return mStack[mCurrentNestingLevel]->enter(name);
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::lookupLocal(const std::string &name) const
{
  return mStack[mCurrentNestingLevel]->lookup(name);
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableStackImpl::lookup(const std::string &name) const
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
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

int SymbolTableImpl::nestingLevel() const
{
  return mNestingLevel;
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl> > SymbolTableImpl::lookup(const std::string &name) const
{
  auto search = mSymbolMap.find(name);
  if (search != mSymbolMap.end()) {
    return search->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>> SymbolTableImpl::enter(const std::string &name)
{
  mSymbolMap[name] = createSymbolTableEntry(name, this);
  return mSymbolMap[name];
}

std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl> > > SymbolTableImpl::sortedEntries() const
{
  // std::map is already sorted
  std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>> result;
  for (auto it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it) {
    result.push_back(it->second);
  }
  return result;
}

SymbolTableEntryImpl::SymbolTableEntryImpl(const std::string &name, SymbolTable<SymbolTableKeyTypeImpl> *symbol_table):
  SymbolTableEntry(name, symbol_table)
{
  mName = name;
  mSymbolTable = symbol_table;
}

SymbolTableEntryImpl::~SymbolTableEntryImpl()
{
//#ifdef DEBUG_DESTRUCTOR
//  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
//#endif
}

std::string SymbolTableEntryImpl::name() const
{
  return mName;
}

SymbolTable<SymbolTableKeyTypeImpl> *SymbolTableEntryImpl::symbolTable() const
{
  return mSymbolTable;
}

void SymbolTableEntryImpl::appendLineNumber(int line_number)
{
  mLineNumbers.push_back(line_number);
}

std::vector<int> SymbolTableEntryImpl::lineNumbers() const
{
  return mLineNumbers;
}

void SymbolTableEntryImpl::setAttribute(const SymbolTableKeyTypeImpl& key, const std::any &value)
{
//  const SymbolTableKeyImpl* key_impl = static_cast<const SymbolTableKeyImpl*>(key);
  mEntryMap[key] = value;
}

std::any SymbolTableEntryImpl::getAttribute(const SymbolTableKeyTypeImpl &key, bool *ok) const
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
    return std::any();
  }
}

ICodeImpl::ICodeImpl(): ICode()
{

}

ICodeImpl::~ICodeImpl()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
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

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeTypeImpl &pType):
  ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(pType), mType(pType), mParent(nullptr)
{

}

ICodeNodeImpl::~ICodeNodeImpl()
{
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ICodeNodeTypeImpl ICodeNodeImpl::type() const
{
  return mType;
}

const ICodeNodeBase *ICodeNodeImpl::parent() const
{
  return mParent;
}

const ICodeNodeBase *ICodeNodeImpl::setParent(const ICodeNodeBase *new_parent)
{
  mParent = new_parent;
  return mParent;
}

std::shared_ptr<ICodeNodeBase> ICodeNodeImpl::addChild(std::shared_ptr<ICodeNodeBase> node)
{
  if (node != nullptr) {
    mChildren.push_back(node);
    mChildren.back()->setParent(this);
  }
  return node;
}

void ICodeNodeImpl::setAttribute(const ICodeKeyTypeImpl &key, const std::any &value)
{
  mHashTable[key] = value;
}

std::any ICodeNodeImpl::getAttribute(const ICodeKeyTypeImpl &key) const
{
  const auto search = mHashTable.find(key);
  if (search != mHashTable.end()) {
    return search->second;
  } else {
    return std::any();
  }
}

std::unique_ptr<ICodeNodeBase> ICodeNodeImpl::copy() const
{
  // only copy this node itself, not the parent and children!
  auto new_node = createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(this->mType);
  auto tmp_ptr = dynamic_cast<ICodeNodeImpl*>(new_node.get());
  for (auto it = mHashTable.cbegin(); it != mHashTable.cend(); ++it) {
    tmp_ptr->mHashTable[it->first] = it->second;
  }
  return std::move(new_node);
}

std::string ICodeNodeImpl::toString() const
{
  switch (mType) {
    case ICodeNodeTypeImpl::PROGRAM: return std::string("PROGRAM");
    case ICodeNodeTypeImpl::PROCEDURE: return std::string("PROCEDURE");
    case ICodeNodeTypeImpl::FUNCTION: return std::string("FUNCTION");
    case ICodeNodeTypeImpl::COMPOUND: return std::string("COMPOUND");
    case ICodeNodeTypeImpl::ASSIGN: return std::string("ASSIGN");
    case ICodeNodeTypeImpl::LOOP: return std::string("LOOP");
    case ICodeNodeTypeImpl::TEST: return std::string("TEST");
    case ICodeNodeTypeImpl::CALL: return std::string("CALL");
    case ICodeNodeTypeImpl::PARAMETERS: return std::string("PARAMETERS");
    case ICodeNodeTypeImpl::IF: return std::string("IF");
    case ICodeNodeTypeImpl::SELECT: return std::string("SELECT");
    case ICodeNodeTypeImpl::SELECT_BRANCH: return std::string("SELECT_BRANCH");
    case ICodeNodeTypeImpl::SELECT_CONSTANTS: return std::string("SELECT_CONSTANTS");
    case ICodeNodeTypeImpl::NO_OP: return std::string("NO_OP");
    case ICodeNodeTypeImpl::EQ: return std::string("EQ");
    case ICodeNodeTypeImpl::NE: return std::string("NE");
    case ICodeNodeTypeImpl::LT: return std::string("LT");
    case ICodeNodeTypeImpl::LE: return std::string("LE");
    case ICodeNodeTypeImpl::GT: return std::string("GT");
    case ICodeNodeTypeImpl::GE: return std::string("GE");
    case ICodeNodeTypeImpl::NOT: return std::string("NOT");
    case ICodeNodeTypeImpl::ADD: return std::string("ADD");
    case ICodeNodeTypeImpl::SUBTRACT: return std::string("SUBTRACT");
    case ICodeNodeTypeImpl::OR: return std::string("OR");
    case ICodeNodeTypeImpl::NEGATE: return std::string("NEGATE");
    case ICodeNodeTypeImpl::MULTIPLY: return std::string("MULTIPLY");
    case ICodeNodeTypeImpl::INTEGER_DIVIDE: return std::string("INTEGER_DIVIDE");
    case ICodeNodeTypeImpl::FLOAT_DIVIDE: return std::string("FLOAT_DIVIDE");
    case ICodeNodeTypeImpl::MOD: return std::string("MOD");
    case ICodeNodeTypeImpl::AND: return std::string("AND");
    case ICodeNodeTypeImpl::VARIABLE: return std::string("VARIABLE");
    case ICodeNodeTypeImpl::SUBSCRIPTS: return std::string("SUBSCRIPTS");
    case ICodeNodeTypeImpl::FIELD: return std::string("FIELD");
    case ICodeNodeTypeImpl::INTEGER_CONSTANT: return std::string("INTEGER_CONSTANT");
    case ICodeNodeTypeImpl::REAL_CONSTANT: return std::string("REAL_CONSTANT");
    case ICodeNodeTypeImpl::STRING_CONSTANT: return std::string("STRING_CONSTANT");
    case ICodeNodeTypeImpl::BOOLEAN_CONSTANT: return std::string("BOOLEAN_CONSTANT");
    default: return std::string("");
  }
}

ICodeNodeImpl::AttributeMapTImpl &ICodeNodeImpl::attributeMap()
{
  return mHashTable;
}

const ICodeNodeImpl::AttributeMapTImpl &ICodeNodeImpl::attributeMap() const
{
  return mHashTable;
}

ICodeNodeImpl::ChildrenContainerTImpl &ICodeNodeImpl::children()
{
  return mChildren;
}

const ICodeNodeImpl::ChildrenContainerTImpl &ICodeNodeImpl::children() const
{
  return mChildren;
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
  const std::string &name, SymbolTable<SymbolTableKeyTypeImpl> *symbolTable)
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
