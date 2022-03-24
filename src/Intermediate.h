#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Common.h"

#include <memory>
#include <any>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
#include <variant>

template <typename KeyT, typename ValueT>
using AttributeMapTImpl = std::unordered_map<KeyT, ValueT>;

template <typename ValueT>
using ChildrenContainerTImpl = std::vector<ValueT>;

template <typename ValueT>
using SymbolStackContainerTImpl = std::vector<ValueT>;

template <auto EnumVal>
auto cast_by_enum(const std::any& x);

template <typename NodeT,
          typename KeyT,
          template <typename...> typename AttributeMapT,
          template <typename...> typename ChildrenContainerT>
class ICodeNode: public std::enable_shared_from_this<ICodeNode<NodeT, KeyT, AttributeMapT, ChildrenContainerT>> {
public:
  using AttributeMapImpl = AttributeMapT<KeyT, std::any>;
  using ChildrenContainerImpl = ChildrenContainerT<std::shared_ptr<ICodeNode>>;
  using attribute_map_iterator = typename AttributeMapT<KeyT, std::any>::iterator;
  using const_attribute_map_iterator = typename AttributeMapT<KeyT, std::any>::const_iterator;
  using children_iterator = typename ChildrenContainerT<std::shared_ptr<ICodeNode>>::iterator;
  using const_children_iterator = typename ChildrenContainerT<std::shared_ptr<ICodeNode>>::const_iterator;
  ICodeNode() {}
  explicit ICodeNode(const NodeT& pType) {}
  virtual ~ICodeNode() {}
  virtual NodeT type() const = 0;
  virtual void setParent(const std::weak_ptr<const ICodeNode>& new_parent) = 0;
  virtual const std::shared_ptr<const ICodeNode> parent() const = 0;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node) = 0;
  virtual void setAttribute(const KeyT& key, const std::any& value) = 0;
  virtual std::any getAttribute(const KeyT& key) const = 0;
  template <KeyT KeyVal>
  auto getAttribute() const {
    return cast_by_enum<KeyVal>(getAttribute(KeyVal));
  }
  virtual std::unique_ptr<ICodeNode> copy() const = 0;
  virtual std::string toString() const = 0;
  virtual attribute_map_iterator attributeMapBegin() = 0;
  virtual attribute_map_iterator attributeMapEnd() = 0;
  virtual const_attribute_map_iterator attributeMapBegin() const = 0;
  virtual const_attribute_map_iterator attributeMapEnd() const = 0;
  virtual children_iterator childrenBegin() = 0;
  virtual children_iterator childrenEnd() = 0;
  virtual const_children_iterator childrenBegin() const = 0;
  virtual const_children_iterator childrenEnd() const = 0;
  virtual size_t numChildren() const = 0;
};

template <typename ICodeNodeType, typename ICodeKeyType,
          template <typename...> typename AttributeMapT,
          template <typename...> typename ChildrenContainerT>
class ICode {
public:
  using ICodeNodeT = ICodeNode<ICodeNodeType, ICodeKeyType, AttributeMapT, ChildrenContainerT>;
  ICode() {}
  virtual ~ICode() {}
  virtual std::shared_ptr<ICodeNodeT> setRoot(std::shared_ptr<ICodeNodeT> node) = 0;
  virtual std::shared_ptr<ICodeNodeT> getRoot() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
class SymbolTable;

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
class TypeSpec;

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
class SymbolTableEntry {
public:
  using SymbolTableT = SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  using AttributeMapImpl = AttributeMapT<SymbolTableKeyT, std::any>;
  using TypeSpecT = TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  SymbolTableEntry(const std::string&, const std::weak_ptr<SymbolTableT>&) {}
  virtual ~SymbolTableEntry() {}
  virtual std::string name() const = 0;
  virtual std::shared_ptr<SymbolTableT> symbolTable() const = 0;
  virtual void appendLineNumber(int line_number) = 0;
  virtual std::vector<int> lineNumbers() const = 0;
  virtual void setAttribute(const SymbolTableKeyT& key, const std::any& value) = 0;
  virtual std::any getAttribute(const SymbolTableKeyT& key, bool* ok = nullptr) const = 0;
  template <SymbolTableKeyT KeyVal>
  auto getAttribute() const {
    return cast_by_enum<KeyVal>(getAttribute(KeyVal));
  }
  virtual void setDefinition(const DefinitionT&) = 0;
  virtual DefinitionT getDefinition() const = 0;
  virtual void setTypeSpec(std::shared_ptr<TypeSpecT> type_spec) = 0;
  virtual std::shared_ptr<TypeSpecT> getTypeSpec() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
class SymbolTable: public std::enable_shared_from_this<SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>> {
public:
  using SymbolTableEntryT = SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  using SymbolTableMapT = AttributeMapT<std::string, std::shared_ptr<SymbolTableEntryT>>;
  explicit SymbolTable(int) {}
  virtual ~SymbolTable() {}
  virtual int nestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTableEntryT> lookup(const std::string& name) const = 0;
  virtual std::shared_ptr<SymbolTableEntryT> enter(const std::string& name) = 0;
  virtual std::vector<std::shared_ptr<SymbolTableEntryT>> sortedEntries() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT,
          template <typename...> typename StackContainerT>
class SymbolTableStack {
public:
  using SymbolTableT = SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  using SymbolTableEntryT = SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  using StackT = StackContainerT<std::shared_ptr<SymbolTableT>>;
  SymbolTableStack() {}
  virtual ~SymbolTableStack() {}
  virtual int currentNestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTableT> localSymbolTable() const = 0;
  virtual std::shared_ptr<SymbolTableEntryT> enterLocal(const std::string& name) = 0;
  virtual std::shared_ptr<SymbolTableEntryT> lookupLocal(const std::string& name) const = 0;
  // lookup an existing symbol table entry throughout the stack
  virtual std::shared_ptr<SymbolTableEntryT> lookup(const std::string& name) const = 0;
  // set the symbol table entry for the main program identifer
  virtual void setProgramId(const std::weak_ptr<SymbolTableEntryT>& entry) = 0;
  // get the symbol table entry for the main program identifier
  virtual std::shared_ptr<SymbolTableEntryT> programId() const = 0;
  // push a new symbol table into the stack,
  // and return the pushed symbol table
  virtual std::shared_ptr<SymbolTableT> push() = 0;
  // overloaded function. Push a symbol table into the stack
  virtual std::shared_ptr<SymbolTableT> push(std::shared_ptr<SymbolTableT>) = 0;
  // pop a symbol table off the stack
  virtual std::shared_ptr<SymbolTableT> pop() = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
class TypeSpec: public std::enable_shared_from_this<TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>> {
public:
  using SymbolTableEntryT = SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>;
  using typespec_map_impl = AttributeMapT<TypeKeyT, std::any>;
  explicit TypeSpec(TypeFormT) {}
  explicit TypeSpec(const std::string&) {}
  virtual ~TypeSpec() {}
  virtual TypeFormT form() const = 0;
  // raw pointers are used to avoid the circular dependency
  virtual void setIdentifier(const std::weak_ptr<SymbolTableEntryT>& identifier) = 0;
  virtual std::shared_ptr<SymbolTableEntryT> getIdentifier() const = 0;
  virtual void setAttribute(TypeKeyT key, const std::any& value) = 0;
  virtual std::any getAttribute(TypeKeyT key) const = 0;
  template <TypeKeyT KeyVal>
  auto getAttribute() const {
    return cast_by_enum<KeyVal>(getAttribute(KeyVal));
  }
  virtual bool isPascalString() const = 0;
  virtual std::shared_ptr<TypeSpec> baseType() = 0;
};

typedef ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, AttributeMapTImpl, ChildrenContainerTImpl> ICodeNodeImplBase;
typedef ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, AttributeMapTImpl, ChildrenContainerTImpl> ICodeImplBase;
typedef SymbolTableEntry<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl> SymbolTableEntryImplBase;
typedef SymbolTable<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl> SymbolTableImplBase;
typedef SymbolTableStack<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl, SymbolStackContainerTImpl> SymbolTableStackImplBase;
typedef TypeSpec<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl, AttributeMapTImpl> TypeSpecImplBase;

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
std::unique_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>>
createSymbolTableEntry(const std::string& name, const std::weak_ptr<SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>>& symbolTable);
std::unique_ptr<SymbolTableEntryImplBase> createSymbolTableEntry(const std::string& name, const std::weak_ptr<SymbolTableImplBase>& symbolTable);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
std::unique_ptr<SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>> createSymbolTable(int nestingLevel);
std::unique_ptr<SymbolTableImplBase> createSymbolTable(int nestingLevel);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT,
          template <typename...> typename StackContainerT>
std::unique_ptr<SymbolTableStack<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT, StackContainerT>> createSymbolTableStack();
std::unique_ptr<SymbolTableStackImplBase> createSymbolTableStack();

template <typename ICodeNodeType, typename ICodeKeyType, template <typename...> typename AttributeMapT, template <typename...> typename ChildrenContainerT>
std::unique_ptr<ICode<ICodeNodeType, ICodeKeyType, AttributeMapT, ChildrenContainerT>> createICode();
std::unique_ptr<ICodeImplBase> createICode();

template <typename NodeT, typename KeyT, template <typename...> typename AttributeMapT, template <typename...> typename ChildrenContainerT>
std::unique_ptr<ICodeNode<NodeT, KeyT, AttributeMapT, ChildrenContainerT>> createICodeNode(const NodeT& type);
std::unique_ptr<ICodeNodeImplBase> createICodeNode(const ICodeNodeTypeImpl &type);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT,
          template <typename...> typename AttributeMapT>
std::unique_ptr<TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT, AttributeMapT>> createType(const TypeFormT& form);
std::unique_ptr<TypeSpecImplBase> createType(const TypeFormImpl& form);
std::unique_ptr<TypeSpecImplBase> createStringType(const std::string& value);

template <SymbolTableKeyTypeImpl> struct SymbolTableKeyToEnum;
template <> struct SymbolTableKeyToEnum<SymbolTableKeyTypeImpl::CONSTANT_VALUE> { using type = std::any; };
template <> struct SymbolTableKeyToEnum<SymbolTableKeyTypeImpl::ROUTINE_SYMTAB> { using type = std::shared_ptr<SymbolTableImplBase>; };
template <> struct SymbolTableKeyToEnum<SymbolTableKeyTypeImpl::ROUTINE_ICODE> { using type = std::shared_ptr<ICodeImplBase>; };
template <> struct SymbolTableKeyToEnum<SymbolTableKeyTypeImpl::ROUTINE_ROUTINES> { using type = std::vector<std::shared_ptr<SymbolTableEntryImplBase>>; };
template <> struct SymbolTableKeyToEnum<SymbolTableKeyTypeImpl::DATA_VALUE> { using type = VariableValueT; };

template <ICodeKeyTypeImpl> struct ICodeKeyTypeImplToEnum;
template <> struct ICodeKeyTypeImplToEnum<ICodeKeyTypeImpl::ID> { using type = std::shared_ptr<SymbolTableEntryImplBase>; };
template <> struct ICodeKeyTypeImplToEnum<ICodeKeyTypeImpl::LINE> { using type = int; };
template <> struct ICodeKeyTypeImplToEnum<ICodeKeyTypeImpl::VALUE> { using type = std::any; };

template <TypeKeyImpl> struct TypeKeyImplToEnum;
template <> struct TypeKeyImplToEnum<TypeKeyImpl::ENUMERATION_CONSTANTS> { using type = std::vector<std::weak_ptr<SymbolTableEntryImplBase>>; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::ARRAY_ELEMENT_COUNT> { using type = PascalInteger; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::ARRAY_ELEMENT_TYPE> { using type = std::shared_ptr<TypeSpecImplBase>; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::ARRAY_INDEX_TYPE> { using type = std::shared_ptr<TypeSpecImplBase>; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::SUBRANGE_BASE_TYPE> { using type = std::shared_ptr<TypeSpecImplBase>; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::SUBRANGE_MIN_VALUE> { using type = std::any; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::SUBRANGE_MAX_VALUE> { using type = std::any; };
template <> struct TypeKeyImplToEnum<TypeKeyImpl::RECORD_SYMTAB> { using type = std::shared_ptr<SymbolTableImplBase>; };

template <auto EnumVal>
constexpr auto EnumToTypeImpl() {
  typedef decltype(EnumVal) enum_type;
  if constexpr (std::is_same_v<enum_type, SymbolTableKeyTypeImpl>) {
    return SymbolTableKeyToEnum<EnumVal>{};
  } else if constexpr (std::is_same_v<enum_type, ICodeKeyTypeImpl>) {
    return ICodeKeyTypeImplToEnum<EnumVal>{};
  } else {
    return TypeKeyImplToEnum<EnumVal>{};
  }
}

template <auto EnumVal>
struct EnumToType {
  using type = typename decltype(EnumToTypeImpl<EnumVal>())::type;
};

template <auto EnumVal>
auto cast_by_enum(const std::any& x) {
  return std::any_cast<typename EnumToType<EnumVal>::type>(x);
}

#endif // INTERMEDIATE_H
