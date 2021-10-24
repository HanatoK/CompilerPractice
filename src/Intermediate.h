#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Common.h"

#include <memory>
#include <any>
#include <vector>
#include <unordered_map>

template <typename NodeT,
          typename KeyT,
          template <typename...> typename AttributeMapT = std::unordered_map,
          template <typename...> typename ChildrenContainerT = std::vector>
class ICodeNode {
public:
  typedef AttributeMapT<KeyT, std::any> AttributeMapTImpl;
  typedef ChildrenContainerT<std::shared_ptr<ICodeNode<NodeT, KeyT>>> ChildrenContainerTImpl;
  typedef typename AttributeMapT<KeyT, std::any>::iterator attribute_map_iterator;
  typedef typename AttributeMapT<KeyT, std::any>::const_iterator const_attribute_map_iterator;
  typedef typename ChildrenContainerT<std::shared_ptr<ICodeNode<NodeT, KeyT>>>::iterator children_iterator;
  typedef typename ChildrenContainerT<std::shared_ptr<ICodeNode<NodeT, KeyT>>>::const_iterator const_children_iterator;
  ICodeNode() {}
  explicit ICodeNode(const NodeT& pType) {}
  virtual ~ICodeNode() {}
  virtual NodeT type() const = 0;
  virtual const ICodeNode* setParent(const ICodeNode* new_parent) = 0;
  virtual const ICodeNode* parent() const = 0;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node) = 0;
  virtual void setAttribute(const KeyT& key, const std::any& value) = 0;
  virtual std::any getAttribute(const KeyT& key) const = 0;
  virtual std::unique_ptr<ICodeNode> copy() const = 0;
  virtual std::string toString() const = 0;
  attribute_map_iterator attributeMapBegin() {
    return attributeMap().begin();
  }
  attribute_map_iterator attributeMapEnd() {
    return attributeMap().end();
  }
  const_attribute_map_iterator attributeMapBegin() const {
    return attributeMap().cbegin();
  }
  const_attribute_map_iterator attributeMapEnd() const {
    return attributeMap().cend();
  }
  children_iterator childrenBegin() {
    return children().begin();
  }
  children_iterator childrenEnd() {
    return children().end();
  }
  const_children_iterator childrenBegin() const {
    return children().cbegin();
  }
  const_children_iterator childrenEnd() const {
    return children().cend();
  }
  size_t numChildren() const {
    return children().size();
  }
protected:
  virtual AttributeMapTImpl& attributeMap() = 0;
  virtual const AttributeMapTImpl& attributeMap() const = 0;
  virtual ChildrenContainerTImpl& children() = 0;
  virtual const ChildrenContainerTImpl& children() const = 0;
};

template <typename ICodeNodeType, typename ICodeKeyType>
class ICode {
public:
  ICode() {}
  virtual ~ICode() {}
  virtual std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> setRoot(std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> node) = 0;
  virtual std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> getRoot() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class SymbolTable;

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class TypeSpec;

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class SymbolTableEntry {
public:
  SymbolTableEntry(const std::string&, SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>*) {}
  virtual ~SymbolTableEntry() {}
  virtual std::string name() const = 0;
  virtual SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>* symbolTable() const = 0;
  virtual void appendLineNumber(int line_number) = 0;
  virtual std::vector<int> lineNumbers() const = 0;
  virtual void setAttribute(const SymbolTableKeyT& key, const std::any& value) = 0;
  virtual std::any getAttribute(const SymbolTableKeyT& key, bool* ok = nullptr) const = 0;
  virtual void setDefinition(const DefinitionT&) = 0;
  virtual DefinitionT getDefinition() const = 0;
  virtual void setTypeSpec(std::shared_ptr<TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> type_spec) = 0;
  virtual std::shared_ptr<TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> getTypeSpec() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class SymbolTable {
public:
  explicit SymbolTable(int) {}
  virtual ~SymbolTable() {}
  virtual int nestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> lookup(const std::string& name) const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> enter(const std::string& name) = 0;
  virtual std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>>> sortedEntries() const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class SymbolTableStack {
public:
  SymbolTableStack() {}
  virtual ~SymbolTableStack() {}
  virtual int currentNestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> localSymbolTable() const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> enterLocal(const std::string& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> lookupLocal(const std::string& name) const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> lookup(const std::string& name) const = 0;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
class TypeSpec {
public:
  explicit TypeSpec(TypeFormT);
  explicit TypeSpec(const std::string&);
  virtual ~TypeSpec() {}
  virtual TypeFormT form() const;
  // raw pointers are used to avoid the circular dependency
  virtual void setIdentifier(SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>* identifier);
  virtual SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>* getIdentifier() const;
  virtual void setAttribute(TypeKeyT key, const std::any& value);
  virtual std::any getAttribute(TypeKeyT key) const;
  virtual bool isPascalString() const;
  TypeSpec baseType() const;
};

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
std::unique_ptr<SymbolTableEntry<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> createSymbolTableEntry(const std::string& name, SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>* symbolTable);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
std::unique_ptr<SymbolTable<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> createSymbolTable(int nestingLevel);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
std::unique_ptr<SymbolTableStack<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> createSymbolTableStack();

template <typename ICodeNodeType, typename ICodeKeyType>
std::unique_ptr<ICode<ICodeNodeType, ICodeKeyType>> createICode();

template <typename ICodeNodeType, typename ICodeKeyType>
std::unique_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> createICodeNode(const ICodeNodeType& type);

template <typename SymbolTableKeyT, typename DefinitionT, typename TypeFormT, typename TypeKeyT>
std::unique_ptr<TypeSpec<SymbolTableKeyT, DefinitionT, TypeFormT, TypeKeyT>> createType(const TypeFormT& form);

#endif // INTERMEDIATE_H
