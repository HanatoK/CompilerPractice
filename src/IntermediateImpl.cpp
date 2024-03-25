#include "IntermediateImpl.h"
#include "Predefined.h"

#include <iostream>
#include <boost/range/adaptor/reversed.hpp>

SymbolTableStackImpl::SymbolTableStackImpl() : SymbolTableStack() {
  mCurrentNestingLevel = 0;
  mStack.push_back(createSymbolTable(0));
}

SymbolTableStackImpl::~SymbolTableStackImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

int SymbolTableStackImpl::currentNestingLevel() const {
 return mCurrentNestingLevel;
  // return static_cast<int>(mStack.size()) - 1;
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
  std::shared_ptr<SymbolTableEntryImplBase> result = nullptr;
  // search the current and enclosing scopes
  for (int i = mCurrentNestingLevel; i >= 0; --i) {
    result = mStack[i]->lookup(name);
    if (result != nullptr) break;
  }
  return result;
}

void SymbolTableStackImpl::setProgramId(const std::weak_ptr<SymbolTableEntryImplBase>& entry)
{
  mProgramId = entry;
}

std::shared_ptr<SymbolTableEntryImplBase> SymbolTableStackImpl::programId() const
{
  return mProgramId.lock();
}

std::shared_ptr<SymbolTableImplBase> SymbolTableStackImpl::push()
{
  auto symbol_table = createSymbolTable(++mCurrentNestingLevel);
  mStack.push_back(std::move(symbol_table));
  return mStack.back();
}

std::shared_ptr<SymbolTableImplBase> SymbolTableStackImpl::push(std::shared_ptr<SymbolTableImplBase> symbol_table)
{
  ++mCurrentNestingLevel;
  mStack.push_back(symbol_table);
  return mStack.back();
}

std::shared_ptr<SymbolTableImplBase> SymbolTableStackImpl::pop()
{
  // auto symbol_table = mStack.back();
  auto symbol_table = mStack.at(mCurrentNestingLevel);
  // mStack.pop_back();
  mStack.erase(mStack.begin() + mCurrentNestingLevel);
 --mCurrentNestingLevel;
  return symbol_table;
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
  mSymbolMap[name] = std::shared_ptr(createSymbolTableEntry(name, weak_from_this()));
  return mSymbolMap[name];
}

std::vector<std::shared_ptr<SymbolTableEntryImplBase> > SymbolTableImpl::sortedEntries() const {
  std::vector<std::shared_ptr<SymbolTableEntryImplBase>> result;
  for (auto it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it) {
    result.push_back(it->second);
  }
  std::sort(result.begin(), result.end(), [](
    const std::shared_ptr<SymbolTableEntryImplBase>& x,
    const std::shared_ptr<SymbolTableEntryImplBase>& y){
      return x->name() < y->name();
    });
  return result;
}

SymbolTableEntryImpl::SymbolTableEntryImpl(const std::string &name, const std::weak_ptr<SymbolTableImplBase>& symbol_table)
    : SymbolTableEntry(name, symbol_table), mSymbolTable(symbol_table), mName(name),
      mDefinition(DefinitionImpl::UNDEFINED), mTypeSpec(nullptr) {
}

SymbolTableEntryImpl::~SymbolTableEntryImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
//  std::cout << "Count: " << mTypeSpec.use_count() << std::endl;
// this pointer is for access only, so don't delete it.
}

std::string SymbolTableEntryImpl::name() const { return mName; }

std::shared_ptr<SymbolTableImplBase> SymbolTableEntryImpl::symbolTable() const {
  return mSymbolTable.lock();
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

std::any SymbolTableEntryImpl::getAttribute(const SymbolTableKeyTypeImpl &key) const {
  //  const SymbolTableKeyImpl* key_impl = static_cast<const
  //  SymbolTableKeyImpl*>(key);
  auto search = mEntryMap.find(key);
  if (search != mEntryMap.end()) {
    return search->second;
  } else {
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

void ICodeImpl::setRoot(const std::shared_ptr<ICodeNodeImplBase>& node) {
  mRoot = node;
}

std::shared_ptr<ICodeNodeImplBase> ICodeImpl::getRoot() const {
  return mRoot;
}

ICodeNodeImpl::ICodeNodeImpl(const ICodeNodeTypeImpl &pType)
    : ICodeNodeImplBase(pType), mType(pType),
      mParent(std::weak_ptr<ICodeNodeImplBase>()), mTypeSpec(nullptr) {}

ICodeNodeImpl::~ICodeNodeImpl() {
#ifdef DEBUG_DESTRUCTOR
  std::cerr << "Destructor: " << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ICodeNodeTypeImpl ICodeNodeImpl::type() const { return mType; }

const std::shared_ptr<const ICodeNodeImplBase> ICodeNodeImpl::parent() const { return mParent.lock(); }

void ICodeNodeImpl::setParent(const std::weak_ptr<const ICodeNodeImplBase>& new_parent) {
  mParent = new_parent;
}

std::shared_ptr<ICodeNodeImplBase>
ICodeNodeImpl::addChild(std::shared_ptr<ICodeNodeImplBase> node) {
  if (node != nullptr) {
    mChildren.push_back(node);
    mChildren.back()->setParent(weak_from_this());
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
  auto new_node = createICodeNode(this->mType);
  auto tmp_ptr = dynamic_cast<ICodeNodeImpl *>(new_node.get());
  for (auto it = mHashTable.cbegin(); it != mHashTable.cend(); ++it) {
    tmp_ptr->mHashTable[it->first] = it->second;
  }
  return new_node;
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

void ICodeNodeImpl::setTypeSpec(const std::shared_ptr<TypeSpecImplBase>& type_spec)
{
  mTypeSpec = type_spec;
}

std::shared_ptr<TypeSpecImplBase> ICodeNodeImpl::getTypeSpec() const
{
  return mTypeSpec;
}

ICodeNodeImpl::AttributeMapImpl &ICodeNodeImpl::attributeMap() {
  return mHashTable;
}

const ICodeNodeImpl::AttributeMapImpl &ICodeNodeImpl::attributeMap() const {
  return mHashTable;
}

ICodeNodeImpl::ChildrenContainerImpl &ICodeNodeImpl::children() {
  return mChildren;
}

const ICodeNodeImpl::ChildrenContainerImpl &ICodeNodeImpl::children() const {
  return mChildren;
}

template <>
std::unique_ptr<ICodeImplBase> createICode() {
  return std::make_unique<ICodeImpl>();
}

std::unique_ptr<ICodeImplBase> createICode() {
  return createICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, AttributeMapTImpl, ChildrenContainerTImpl, TypeSpecImplBase>();
}

template <>
std::unique_ptr<ICodeNodeImplBase> createICodeNode(const ICodeNodeTypeImpl &type) {
  return std::make_unique<ICodeNodeImpl>(type);
}

std::unique_ptr<ICodeNodeImplBase> createICodeNode(const ICodeNodeTypeImpl &type) {
  return createICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, AttributeMapTImpl, ChildrenContainerTImpl, TypeSpecImplBase>(type);
}

template <>
std::unique_ptr<SymbolTableEntryImplBase> createSymbolTableEntry(const std::string &name,
                       const std::weak_ptr<SymbolTableImplBase>& symbolTable) {
  return std::make_unique<SymbolTableEntryImpl>(name, symbolTable);
}

std::unique_ptr<SymbolTableEntryImplBase> createSymbolTableEntry(const std::string &name,
                       const std::weak_ptr<SymbolTableImplBase>& symbolTable) {
  return createSymbolTableEntry<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl>(name, symbolTable);
}

template <>
std::unique_ptr<SymbolTableImplBase>
createSymbolTable(int nestingLevel) {
  return std::make_unique<SymbolTableImpl>(nestingLevel);
}

std::unique_ptr<SymbolTableImplBase>
createSymbolTable(int nestingLevel) {
  return createSymbolTable<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl>(nestingLevel);
}

template <>
std::unique_ptr<SymbolTableStackImplBase> createSymbolTableStack() {
  return std::make_unique<SymbolTableStackImpl>();
}

std::unique_ptr<SymbolTableStackImplBase> createSymbolTableStack() {
  return createSymbolTableStack<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl, SymbolStackContainerTImpl>();
}

TypeSpecImpl::TypeSpecImpl(TypeFormImpl form)
    : TypeSpecImplBase(form),
      mForm(form), mIdentifier() {}

TypeSpecImpl::TypeSpecImpl(const std::string& value)
    : TypeSpecImplBase(value), mIdentifier() {
  mForm = TypeFormImpl::ARRAY;
  std::shared_ptr<TypeSpecImplBase> index_type = createType(TypeFormImpl::SUBRANGE);
  index_type->setAttribute<TypeKeyImpl::SUBRANGE_BASE_TYPE>(Predefined::instance().integerType);
  index_type->setAttribute<TypeKeyImpl::SUBRANGE_MIN_VALUE>(VariableValueT{1ll});
  index_type->setAttribute<TypeKeyImpl::SUBRANGE_MAX_VALUE>(VariableValueT{static_cast<PascalInteger>(value.size())});
  TypeSpecImpl::setAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE, index_type);
  TypeSpecImpl::setAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE, Predefined::instance().charType);
  TypeSpecImpl::setAttribute(TypeKeyImpl::ARRAY_ELEMENT_COUNT, static_cast<PascalInteger>(value.size()));
}

TypeSpecImpl::~TypeSpecImpl()
{
}

TypeFormImpl TypeSpecImpl::form() const
{
  return mForm;
}

void TypeSpecImpl::setIdentifier(const std::weak_ptr<SymbolTableEntryT>& identifier)
{
  mIdentifier = identifier;
}

std::shared_ptr<SymbolTableEntryImplBase> TypeSpecImpl::getIdentifier() const
{
  return mIdentifier.lock();
}

void TypeSpecImpl::setAttribute(TypeKeyImpl key, const std::any& value)
{
  mTypeSpecMap[key] = value;
}

std::any TypeSpecImpl::getAttribute(TypeKeyImpl key) const
{
  auto search = mTypeSpecMap.find(key);
  if (search != mTypeSpecMap.end()) {
    return search->second;
  } else {
    return nullptr;
  }
}

bool TypeSpecImpl::isPascalString() const
{
  if (mForm == TypeFormImpl::ARRAY) {
    const auto element_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::ARRAY_ELEMENT_TYPE));
    const auto index_type = std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::ARRAY_INDEX_TYPE));
    if (element_type->baseType() == Predefined::instance().charType &&
        index_type->baseType() == Predefined::instance().integerType) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

std::shared_ptr<TypeSpecImplBase> TypeSpecImpl::baseType()
{
  if (mForm == TypeFormImpl::SUBRANGE) {
    return std::any_cast<std::shared_ptr<TypeSpecImplBase>>(getAttribute(TypeKeyImpl::SUBRANGE_BASE_TYPE));
  } else {
    return shared_from_this();
  }
}

std::string TypeSpecImpl::anonymousName() const {
  std::string type_form_name;
  switch (form()) {
    case TypeFormImpl::SCALAR: type_form_name += "scalar"; break;
    case TypeFormImpl::ENUMERATION: type_form_name += "enum"; break;
    case TypeFormImpl::SUBRANGE: type_form_name += "subrange"; break;
    case TypeFormImpl::ARRAY: type_form_name += "array"; break;
    case TypeFormImpl::RECORD: type_form_name += "record"; break;
  }
  auto mangled_name = std::to_string(type_form_name.size()) + type_form_name;
  switch (form()) {
    case TypeFormImpl::SCALAR: {
      // nothing special for scalar type
      break;
    }
    case TypeFormImpl::ENUMERATION: {
      mangled_name += "N";
      const auto enum_constant = TypeSpecImplBase::getAttribute<TypeKeyImpl::ENUMERATION_CONSTANTS>();
      for (const auto& it_enum_item : enum_constant) {
        const auto item_name = it_enum_item.lock()->name();
        mangled_name += std::to_string(item_name.size()) + item_name;
      }
      mangled_name += "E";
      break;
    }
    case TypeFormImpl::SUBRANGE: {
      mangled_name += "N";
      const auto subrange_base_type = TypeSpecImplBase::getAttribute<TypeKeyImpl::SUBRANGE_BASE_TYPE>();
      mangled_name += subrange_base_type->anonymousName();
      mangled_name += "E";
      break;
    }
    case TypeFormImpl::ARRAY: {
      mangled_name += "N";
      const auto array_index_type = TypeSpecImplBase::getAttribute<TypeKeyImpl::ARRAY_INDEX_TYPE>();
      mangled_name += "N";
      mangled_name += array_index_type->anonymousName();
      mangled_name += "E";
      mangled_name += "N";
      const auto array_element_count = TypeSpecImplBase::getAttribute<TypeKeyImpl::ARRAY_ELEMENT_COUNT>();
      mangled_name += std::to_string(array_element_count);
      const auto array_element_type = TypeSpecImplBase::getAttribute<TypeKeyImpl::ARRAY_ELEMENT_TYPE>();
      mangled_name += array_element_type->anonymousName();
      mangled_name += "E";
      mangled_name += "E";
      break;
    }
    case TypeFormImpl::RECORD: {
      // nothing special for record type
      break;
    }
  }
  return mangled_name;
}

template <>
std::unique_ptr<TypeSpecImplBase> createType(const TypeFormImpl& form)
{
  return std::make_unique<TypeSpecImpl>(form);
}

std::unique_ptr<TypeSpecImplBase> createType(const TypeFormImpl& form) {
  return createType<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl>(form);
}

std::unique_ptr<TypeSpecImplBase> createStringType(const std::string& value) {
  return std::make_unique<TypeSpecImpl>(value);
}
