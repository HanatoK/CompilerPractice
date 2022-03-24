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
  virtual ~ICodeNodeImpl();
  virtual ICodeNodeTypeImpl type() const override;
  virtual const std::shared_ptr<const ICodeNodeImplBase> parent() const override;
  virtual void setParent(const std::weak_ptr<const ICodeNodeImplBase>& new_parent) override;
  virtual std::shared_ptr<ICodeNodeImplBase> addChild(std::shared_ptr<ICodeNodeImplBase> node) override;
  virtual void setAttribute(const ICodeKeyTypeImpl& key, const std::any& value) override;
  virtual std::any getAttribute(const ICodeKeyTypeImpl& key) const override;
  virtual std::unique_ptr<ICodeNodeImplBase> copy() const override;
  virtual std::string toString() const override;
  virtual attribute_map_iterator attributeMapBegin() override {
    return attributeMap().begin();
  }
  virtual attribute_map_iterator attributeMapEnd() override {
    return attributeMap().end();
  }
  virtual const_attribute_map_iterator attributeMapBegin() const override {
    return attributeMap().cbegin();
  }
  virtual const_attribute_map_iterator attributeMapEnd() const override {
    return attributeMap().cend();
  }
  virtual children_iterator childrenBegin() override {
    return children().begin();
  }
  virtual children_iterator childrenEnd() override {
    return children().end();
  }
  virtual const_children_iterator childrenBegin() const override {
    return children().cbegin();
  }
  virtual const_children_iterator childrenEnd() const override {
    return children().cend();
  }
  virtual size_t numChildren() const override {
    return children().size();
  }
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
};

class ICodeImpl : public ICodeImplBase {
public:
  ICodeImpl();
  virtual ~ICodeImpl();
  virtual std::shared_ptr<ICodeNodeImplBase> setRoot(std::shared_ptr<ICodeNodeImplBase> node) override;
  virtual std::shared_ptr<ICodeNodeImplBase> getRoot() const override;

private:
  std::shared_ptr<ICodeNodeImplBase> mRoot;
};

class SymbolTableEntryImpl : public SymbolTableEntryImplBase {
public:
  SymbolTableEntryImpl(const std::string &name, const std::weak_ptr<SymbolTableImplBase>& symbol_table);
  virtual ~SymbolTableEntryImpl();
  virtual std::string name() const override;
  virtual std::shared_ptr<SymbolTableImplBase> symbolTable() const override;
  virtual void appendLineNumber(int line_number) override;
  virtual std::vector<int> lineNumbers() const override;
  virtual void setAttribute(const SymbolTableKeyTypeImpl &key, const std::any &value) override;
  virtual std::any getAttribute(const SymbolTableKeyTypeImpl &key, bool *ok = nullptr) const override;
  virtual void setDefinition(const DefinitionImpl& definition) override;
  virtual DefinitionImpl getDefinition() const override;
  virtual void setTypeSpec(std::shared_ptr<TypeSpecImplBase> type_spec) override;
  virtual std::shared_ptr<TypeSpecImplBase> getTypeSpec() const override;
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
  virtual ~SymbolTableImpl();
  virtual int nestingLevel() const override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> enter(const std::string &name) override;
  virtual std::vector<std::shared_ptr<SymbolTableEntryImplBase>> sortedEntries() const override;
private:
  int mNestingLevel;
  SymbolTableMapT mSymbolMap;
};

class SymbolTableStackImpl : public SymbolTableStackImplBase {
public:
  SymbolTableStackImpl();
  virtual ~SymbolTableStackImpl();
  virtual int currentNestingLevel() const override;
  virtual std::shared_ptr<SymbolTableImplBase> localSymbolTable() const override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> enterLocal(const std::string &name) override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookupLocal(const std::string &name) const override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const override;
  virtual void setProgramId(const std::weak_ptr<SymbolTableEntryImplBase>& entry) override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> programId() const override;
  virtual std::shared_ptr<SymbolTableImplBase> push() override;
  virtual std::shared_ptr<SymbolTableImplBase> push(std::shared_ptr<SymbolTableImplBase> symbol_table) override;
  virtual std::shared_ptr<SymbolTableImplBase> pop() override;
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
  virtual ~TypeSpecImpl();
  virtual TypeFormImpl form() const override;
  virtual void setIdentifier(const std::weak_ptr<SymbolTableEntryImplBase>& identifier) override;
  virtual std::shared_ptr<SymbolTableEntryImplBase> getIdentifier() const override;
  virtual void setAttribute(TypeKeyImpl key, const std::any& value) override;
  virtual std::any getAttribute(TypeKeyImpl key) const override;
  virtual bool isPascalString() const override;
  virtual std::shared_ptr<TypeSpecImplBase> baseType() override;
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
