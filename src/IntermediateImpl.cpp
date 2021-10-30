#include "IntermediateImpl.h"
#include "Predefined.h"

#include <iostream>

SymbolTableStackImpl::SymbolTableStackImpl() : SymbolTableStack() {
  mCurrentNestingLevel = 0;
  mStack.push_back(
      createSymbolTable<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>(mCurrentNestingLevel));
}

SymbolTableStackImpl::~SymbolTableStackImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

int SymbolTableStackImpl::currentNestingLevel() const {
  return mCurrentNestingLevel;
}

std::shared_ptr<SymbolTableImplBase> SymbolTableStackImpl::localSymbolTable() const {
  return mStack[mCurrentNestingLevel];
}

std::shared_ptr<SymbolTableEntryImplBase> SymbolTableStackImpl::enterLocal(const std::string &name) {
  return mStack[mCurrentNestingLevel]->enter(name);
}

std::shared_ptr<SymbolTableEntryImplBase>
SymbolTableStackImpl::lookupLocal(const std::string &name) const {
  return mStack[mCurrentNestingLevel]->lookup(name);
}

std::shared_ptr<SymbolTableEntryImplBase> SymbolTableStackImpl::lookup(const std::string &name) const {
  return lookupLocal(name);
}

SymbolTableImpl::SymbolTableImpl(int nesting_level)
    : SymbolTable(nesting_level) {
  mNestingLevel = nesting_level;
}

SymbolTableImpl::~SymbolTableImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

int SymbolTableImpl::nestingLevel() const { return mNestingLevel; }

std::shared_ptr<SymbolTableEntryImplBase> SymbolTableImpl::lookup(const std::string &name) const {
  auto search = mSymbolMap.find(name);
  if (search != mSymbolMap.end()) {
    return search->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SymbolTableEntryImplBase>
SymbolTableImpl::enter(const std::string &name) {
  mSymbolMap[name] = createSymbolTableEntry(name, this);
  return mSymbolMap[name];
}

std::vector<std::shared_ptr<SymbolTableEntryImplBase> > SymbolTableImpl::sortedEntries() const {
  // std::map is already sorted
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> result;
  for (auto it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it) {
    result.push_back(it->second);
  }
  return result;
}

SymbolTableEntryImpl::SymbolTableEntryImpl(const std::string &name, SymbolTableImplBase* symbol_table)
    : SymbolTableEntry(name, symbol_table), mName(name), mDefinition(DefinitionImpl::UNDEFINED), mTypeSpec(nullptr) {
  mSymbolTable = symbol_table;
}

SymbolTableEntryImpl::~SymbolTableEntryImpl() {
  //#ifdef DEBUG_DESTRUCTOR
  //  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
  //#endif
  // this pointer is for access only, so don't delete it.
  mSymbolTable = nullptr;
}

std::string SymbolTableEntryImpl::name() const { return mName; }

SymbolTableImplBase* SymbolTableEntryImpl::symbolTable() const {
  return mSymbolTable;
}

void SymbolTableEntryImpl::appendLineNumber(int line_number) {
  mLineNumbers.push_back(line_number);
}

std::vector<int> SymbolTableEntryImpl::lineNumbers() const {
  return mLineNumbers;
}

void SymbolTableEntryImpl::setAttribute(const SymbolTableKeyTypeImpl &key,
                                        const std::any &value) {
  //  const SymbolTableKeyImpl* key_impl = static_cast<const
  //  SymbolTableKeyImpl*>(key);
  mEntryMap[key] = value;
}

std::any SymbolTableEntryImpl::getAttribute(const SymbolTableKeyTypeImpl &key,
                                            bool *ok) const {
  //  const SymbolTableKeyImpl* key_impl = static_cast<const
  //  SymbolTableKeyImpl*>(key);
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
    return std::any{};
  }
}

void SymbolTableEntryImpl::setDefinition(const DefinitionImpl& definition)
{
  mDefinition = definition;
}

DefinitionImpl SymbolTableEntryImpl::getDefinition() const
{
  return mDefinition;
}

void SymbolTableEntryImpl::setTypeSpec(std::shared_ptr<TypeSpecImplBase> type_spec)
{
  mTypeSpec = type_spec;
}

std::shared_ptr<TypeSpecImplBase> SymbolTableEntryImpl::getTypeSpec() const
{
  return mTypeSpec;
}

ICodeImpl::ICodeImpl() : ICode() {}

ICodeImpl::~ICodeImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

std::shared_ptr<ICodeNodeImplBase> ICodeImpl::setRoot(std::shared_ptr<ICodeNodeImplBase> node) {
  mRoot = node;
  return getRoot();
}

std::shared_ptr<ICodeNodeImplBase> ICodeImpl::getRoot() const {
  return mRoot;
}

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeTypeImpl &pType)
    : ICodeNodeImplBase(pType), mType(pType),
      mParent(nullptr) {}

ICodeNodeImpl::~ICodeNodeImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ICodeNodeTypeImpl ICodeNodeImpl::type() const { return mType; }

const ICodeNodeImplBase *ICodeNodeImpl::parent() const { return mParent; }

const ICodeNodeImplBase *ICodeNodeImpl::setParent(const ICodeNodeImplBase *new_parent) {
  mParent = new_parent;
  return mParent;
}

std::shared_ptr<ICodeNodeImplBase>
ICodeNodeImpl::addChild(std::shared_ptr<ICodeNodeImplBase> node) {
  if (node != nullptr) {
    mChildren.push_back(node);
    mChildren.back()->setParent(this);
  }
  return node;
}

void ICodeNodeImpl::setAttribute(const ICodeKeyTypeImpl &key,
                                 const std::any &value) {
  mHashTable[key] = value;
}

std::any ICodeNodeImpl::getAttribute(const ICodeKeyTypeImpl &key) const {
  const auto search = mHashTable.find(key);
  if (search != mHashTable.end()) {
    return search->second;
  } else {
    return std::any();
  }
}

std::unique_ptr<ICodeNodeImplBase> ICodeNodeImpl::copy() const {
  // only copy this node itself, not the parent and children!
  auto new_node =
      createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>(this->mType);
  auto tmp_ptr = dynamic_cast<ICodeNodeImpl *>(new_node.get());
  for (auto it = mHashTable.cbegin(); it != mHashTable.cend(); ++it) {
    tmp_ptr->mHashTable[it->first] = it->second;
  }
  return std::move(new_node);
}

std::string ICodeNodeImpl::toString() const {
  switch (mType) {
  case ICodeNodeTypeImpl::PROGRAM:
    return std::string("PROGRAM");
  case ICodeNodeTypeImpl::PROCEDURE:
    return std::string("PROCEDURE");
  case ICodeNodeTypeImpl::FUNCTION:
    return std::string("FUNCTION");
  case ICodeNodeTypeImpl::COMPOUND:
    return std::string("COMPOUND");
  case ICodeNodeTypeImpl::ASSIGN:
    return std::string("ASSIGN");
  case ICodeNodeTypeImpl::LOOP:
    return std::string("LOOP");
  case ICodeNodeTypeImpl::TEST:
    return std::string("TEST");
  case ICodeNodeTypeImpl::CALL:
    return std::string("CALL");
  case ICodeNodeTypeImpl::PARAMETERS:
    return std::string("PARAMETERS");
  case ICodeNodeTypeImpl::IF:
    return std::string("IF");
  case ICodeNodeTypeImpl::SELECT:
    return std::string("SELECT");
  case ICodeNodeTypeImpl::SELECT_BRANCH:
    return std::string("SELECT_BRANCH");
  case ICodeNodeTypeImpl::SELECT_CONSTANTS:
    return std::string("SELECT_CONSTANTS");
  case ICodeNodeTypeImpl::NO_OP:
    return std::string("NO_OP");
  case ICodeNodeTypeImpl::EQ:
    return std::string("EQ");
  case ICodeNodeTypeImpl::NE:
    return std::string("NE");
  case ICodeNodeTypeImpl::LT:
    return std::string("LT");
  case ICodeNodeTypeImpl::LE:
    return std::string("LE");
  case ICodeNodeTypeImpl::GT:
    return std::string("GT");
  case ICodeNodeTypeImpl::GE:
    return std::string("GE");
  case ICodeNodeTypeImpl::NOT:
    return std::string("NOT");
  case ICodeNodeTypeImpl::ADD:
    return std::string("ADD");
  case ICodeNodeTypeImpl::SUBTRACT:
    return std::string("SUBTRACT");
  case ICodeNodeTypeImpl::OR:
    return std::string("OR");
  case ICodeNodeTypeImpl::NEGATE:
    return std::string("NEGATE");
  case ICodeNodeTypeImpl::MULTIPLY:
    return std::string("MULTIPLY");
  case ICodeNodeTypeImpl::INTEGER_DIVIDE:
    return std::string("INTEGER_DIVIDE");
  case ICodeNodeTypeImpl::FLOAT_DIVIDE:
    return std::string("FLOAT_DIVIDE");
  case ICodeNodeTypeImpl::MOD:
    return std::string("MOD");
  case ICodeNodeTypeImpl::AND:
    return std::string("AND");
  case ICodeNodeTypeImpl::VARIABLE:
    return std::string("VARIABLE");
  case ICodeNodeTypeImpl::SUBSCRIPTS:
    return std::string("SUBSCRIPTS");
  case ICodeNodeTypeImpl::FIELD:
    return std::string("FIELD");
  case ICodeNodeTypeImpl::INTEGER_CONSTANT:
    return std::string("INTEGER_CONSTANT");
  case ICodeNodeTypeImpl::REAL_CONSTANT:
    return std::string("REAL_CONSTANT");
  case ICodeNodeTypeImpl::STRING_CONSTANT:
    return std::string("STRING_CONSTANT");
  case ICodeNodeTypeImpl::BOOLEAN_CONSTANT:
    return std::string("BOOLEAN_CONSTANT");
  default:
    return std::string("");
  }
}

ICodeNodeImpl::AttributeMapTImpl &ICodeNodeImpl::attributeMap() {
  return mHashTable;
}

const ICodeNodeImpl::AttributeMapTImpl &ICodeNodeImpl::attributeMap() const {
  return mHashTable;
}

ICodeNodeImpl::ChildrenContainerTImpl &ICodeNodeImpl::children() {
  return mChildren;
}

const ICodeNodeImpl::ChildrenContainerTImpl &ICodeNodeImpl::children() const {
  return mChildren;
}

template <>
std::unique_ptr<ICodeImplBase> createICode() {
  return std::make_unique<ICodeImpl>();
}

template <>
std::unique_ptr<ICodeNodeImplBase> createICodeNode(const ICodeNodeTypeImpl &type) {
  return std::make_unique<ICodeNodeImpl>(type);
}

template <>
std::unique_ptr<SymbolTableEntryImplBase> createSymbolTableEntry(const std::string &name,
                       SymbolTableImplBase* symbolTable) {
  return std::make_unique<SymbolTableEntryImpl>(name, symbolTable);
}

template <>
std::unique_ptr<SymbolTableImplBase>
createSymbolTable(int nestingLevel) {
  return std::make_unique<SymbolTableImpl>(nestingLevel);
}

template <>
std::unique_ptr<SymbolTableStackImplBase> createSymbolTableStack() {
  return std::make_unique<SymbolTableStackImpl>();
}

TypeSpecImpl::TypeSpecImpl(TypeFormImpl form)
    : TypeSpecImplBase(form),
      mForm(form), mIdentifier(nullptr) {}

TypeSpecImpl::TypeSpecImpl(const std::string& value)
    : TypeSpecImplBase(value), mIdentifier(nullptr) {
  mForm = TypeFormImpl::ARRAY;
  TypeSpecImpl index_type(TypeFormImpl::SUBRANGE);
  // TODO: predefined types
  index_type.setAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE, Predefined::instance().integerType);
  index_type.setAttribute(TypeKeyImpl::SUBRANGE_MIN_VALUE, 1ll);
  index_type.setAttribute(TypeKeyImpl::SUBRANGE_MAX_VALUE, static_cast<long long>(value.size()));
  setAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE, index_type);
  setAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE, Predefined::instance().charType);
  setAttribute(TypeKeyImpl::ARRAY_ELEMENT_COUNT, static_cast<long long>(value.size()));
}

TypeSpecImpl::~TypeSpecImpl()
{
  mIdentifier = nullptr;
}

TypeFormImpl TypeSpecImpl::form() const
{
  return mForm;
}

void TypeSpecImpl::setIdentifier(SymbolTableEntryImplBase* identifier)
{
  mIdentifier = identifier;
}

SymbolTableEntryImplBase* TypeSpecImpl::getIdentifier() const
{
  return mIdentifier;
}

void TypeSpecImpl::setAttribute(TypeKeyImpl key, const std::any& value)
{
  mTypeSpecMap[key] = value;
}

std::any TypeSpecImpl::getAttribute(TypeKeyImpl key) const
{
  auto search = mTypeSpecMap.find(key);
  if (search != mTypeSpecMap.end()) {
    return *search;
  } else {
    return nullptr;
  }
}

bool TypeSpecImpl::isPascalString() const
{
  if (mForm == TypeFormImpl::ARRAY) {
    const auto element_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::ARRAY_ELEMENT_COUNT));
    const auto index_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE));
    if (element_type == Predefined::instance().charType &&
        index_type == Predefined::instance().integerType) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

TypeSpecImplBase* TypeSpecImpl::baseType()
{
  if (mForm == TypeFormImpl::SUBRANGE) {
    return std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE)).get();
  } else {
    return this;
  }
}

// TODO: figure out why this does not work
//std::unique_ptr<TypeSpec<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>> createType(const TypeFormImpl& form)
//{
//  return std::make_unique<TypeSpecImpl>(form);
//}
