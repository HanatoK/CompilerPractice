#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Common.h"
#include "Intermediate.h"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

class ICodeNodeImpl : public ICodeNodeImplBase {
public:
  explicit ICodeNodeImpl(const ICodeNodeTypeImpl &pType);
  ~ICodeNodeImpl() override;
  ICodeNodeTypeImpl type() const override;
  [[nodiscard]] const std::shared_ptr<const ICodeNodeImplBase> parent() const override;
  void setParent(const std::weak_ptr<const ICodeNodeImplBase>& new_parent) override;
  std::shared_ptr<ICodeNodeImplBase> addChild(std::shared_ptr<ICodeNodeImplBase> node) override;
  void setAttribute(const ICodeKeyTypeImpl& key, const std::any& value) override;
  [[nodiscard]] std::any getAttribute(const ICodeKeyTypeImpl& key) const override;
  [[nodiscard]] std::unique_ptr<ICodeNodeImplBase> copy() const override;
  [[nodiscard]] std::string toString() const override;
  attribute_map_iterator attributeMapBegin() override {
    return attributeMap().begin();
  }
  attribute_map_iterator attributeMapEnd() override {
    return attributeMap().end();
  }
  const_attribute_map_iterator attributeMapBegin() const override {
    return attributeMap().cbegin();
  }
  const_attribute_map_iterator attributeMapEnd() const override {
    return attributeMap().cend();
  }
  children_iterator childrenBegin() override {
    return children().begin();
  }
  children_iterator childrenEnd() override {
    return children().end();
  }
  const_children_iterator childrenBegin() const override {
    return children().cbegin();
  }
  const_children_iterator childrenEnd() const override {
    return children().cend();
  }
  size_t numChildren() const override {
    return children().size();
  }
  void setTypeSpec(const std::shared_ptr<TypeSpecImplBase>& type_spec) override;
  [[nodiscard]] std::shared_ptr<TypeSpecImplBase> getTypeSpec() const override;
protected:
  virtual AttributeMapImpl& attributeMap();
  virtual const AttributeMapImpl& attributeMap() const;
  virtual ChildrenContainerImpl& children();
  virtual const ChildrenContainerImpl& children() const;
private:
  ICodeNodeTypeImpl mType;
  std::weak_ptr<const ICodeNodeImplBase> mParent;
  AttributeMapImpl mHashTable;
  ChildrenContainerImpl mChildren;
  std::shared_ptr<TypeSpecImplBase> mTypeSpec;
};

class ICodeImpl : public ICodeImplBase {
public:
  ICodeImpl();
  ~ICodeImpl() override;
  void setRoot(const std::shared_ptr<ICodeNodeImplBase>& node) override;
  [[nodiscard]] std::shared_ptr<ICodeNodeImplBase> getRoot() const override;

private:
  std::shared_ptr<ICodeNodeImplBase> mRoot;
};

class SymbolTableEntryImpl : public SymbolTableEntryImplBase {
public:
  SymbolTableEntryImpl(const std::string &name, const std::weak_ptr<SymbolTableImplBase>& symbol_table);
  ~SymbolTableEntryImpl() override;
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::shared_ptr<SymbolTableImplBase> symbolTable() const override;
  void appendLineNumber(int line_number) override;
  [[nodiscard]] std::vector<int> lineNumbers() const override;
  void setAttribute(const SymbolTableKeyTypeImpl &key, const std::any &value) override;
  [[nodiscard]] std::any getAttribute(const SymbolTableKeyTypeImpl &key) const override;
  void setDefinition(const DefinitionImpl& definition) override;
  [[nodiscard]] DefinitionImpl getDefinition() const override;
  void setTypeSpec(std::shared_ptr<TypeSpecImplBase> type_spec) override;
  [[nodiscard]] std::shared_ptr<TypeSpecImplBase> getTypeSpec() const override;
private:
  std::weak_ptr<SymbolTableImplBase> mSymbolTable;
  std::vector<int> mLineNumbers;
  std::string mName;
  AttributeMapImpl mEntryMap;
  DefinitionImpl mDefinition;
  std::shared_ptr<TypeSpecImplBase> mTypeSpec;
};

class SymbolTableImpl : public SymbolTableImplBase {
public:
  explicit SymbolTableImpl(int nestingLevel);
  ~SymbolTableImpl() override;
  [[nodiscard]] int nestingLevel() const override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const override;
  std::shared_ptr<SymbolTableEntryImplBase> enter(const std::string &name) override;
  [[nodiscard]] std::vector<std::shared_ptr<SymbolTableEntryImplBase>> sortedEntries() const override;
private:
  int mNestingLevel;
  SymbolTableMapT mSymbolMap;
};

class SymbolTableStackImpl : public SymbolTableStackImplBase {
public:
  SymbolTableStackImpl();
  ~SymbolTableStackImpl() override;
  [[nodiscard]] int currentNestingLevel() const override;
  [[nodiscard]] std::shared_ptr<SymbolTableImplBase> localSymbolTable() const override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> enterLocal(const std::string &name) override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> lookupLocal(const std::string &name) const override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const override;
  void setProgramId(const std::weak_ptr<SymbolTableEntryImplBase>& entry) override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> programId() const override;
  std::shared_ptr<SymbolTableImplBase> push() override;
  std::shared_ptr<SymbolTableImplBase> push(std::shared_ptr<SymbolTableImplBase> symbol_table) override;
  std::shared_ptr<SymbolTableImplBase> pop() override;
private:
  // why is it necessary to maintain a standalone variable for recording the nesting level?
  // Isn't mStack.size() enough?
//  int mCurrentNestingLevel;
  StackT mStack;
  // entry for the main program identifier
  std::weak_ptr<SymbolTableEntryImplBase> mProgramId;
};

class TypeSpecImpl : public TypeSpecImplBase {
public:
  explicit TypeSpecImpl(TypeFormImpl form);
  explicit TypeSpecImpl(const std::string& value);
  ~TypeSpecImpl() override;
  [[nodiscard]] TypeFormImpl form() const override;
  void setIdentifier(const std::weak_ptr<SymbolTableEntryImplBase>& identifier) override;
  [[nodiscard]] std::shared_ptr<SymbolTableEntryImplBase> getIdentifier() const override;
  void setAttribute(TypeKeyImpl key, const std::any& value) override;
  [[nodiscard]] std::any getAttribute(TypeKeyImpl key) const override;
  [[nodiscard]] bool isPascalString() const override;
  [[nodiscard]] std::shared_ptr<TypeSpecImplBase> baseType() override;
private:
  TypeFormImpl mForm;
  std::weak_ptr<SymbolTableEntryImplBase> mIdentifier;
  typespec_map_impl mTypeSpecMap;
};

template <>
std::unique_ptr<SymbolTableEntryImplBase>
createSymbolTableEntry(const std::string &name,
                       const std::weak_ptr<SymbolTableImplBase>& symbolTable);

template <>
std::unique_ptr<SymbolTableImplBase> createSymbolTable(int nestingLevel);

template <>
std::unique_ptr<SymbolTableStackImplBase> createSymbolTableStack();

template <>
std::unique_ptr<ICodeImplBase> createICode();

template <>
std::unique_ptr<ICodeNodeImplBase> createICodeNode(const ICodeNodeTypeImpl &type);

template <>
std::unique_ptr<TypeSpecImplBase> createType(const TypeFormImpl& form);

#endif // INTERMEDIATEIMPL_H
