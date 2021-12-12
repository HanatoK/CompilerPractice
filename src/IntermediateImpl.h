#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Common.h"
#include "Intermediate.h"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

typedef ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, std::unordered_map, std::vector> ICodeNodeImplBase;
typedef ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> ICodeImplBase;
typedef SymbolTableEntry<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl> SymbolTableEntryImplBase;
typedef SymbolTable<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl> SymbolTableImplBase;
typedef SymbolTableStack<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl> SymbolTableStackImplBase;
typedef TypeSpec<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl> TypeSpecImplBase;

class ICodeNodeImpl : public ICodeNodeImplBase {
public:
  explicit ICodeNodeImpl(const ICodeNodeTypeImpl &pType);
  virtual ~ICodeNodeImpl();
  virtual ICodeNodeTypeImpl type() const override;
  virtual const ICodeNodeImplBase* parent() const override;
  virtual const ICodeNodeImplBase* setParent(const ICodeNodeImplBase* new_parent) override;
  virtual std::shared_ptr<ICodeNodeImplBase> addChild(std::shared_ptr<ICodeNodeImplBase> node) override;
  virtual void setAttribute(const ICodeKeyTypeImpl& key, const std::any& value) override;
  virtual std::any getAttribute(const ICodeKeyTypeImpl& key) const override;
  virtual std::unique_ptr<ICodeNodeImplBase> copy() const override;
  virtual std::string toString() const override;
protected:
  virtual AttributeMapTImpl& attributeMap() override;
  virtual const AttributeMapTImpl& attributeMap() const override;
  virtual ChildrenContainerTImpl& children() override;
  virtual const ChildrenContainerTImpl& children() const override;
private:
  ICodeNodeTypeImpl mType;
  // TODO: do I need to use weak pointer here?
  const ICodeNodeImplBase *mParent;
  AttributeMapTImpl mHashTable;
  ChildrenContainerTImpl mChildren;
};

class ICodeImpl : public ICodeImplBase {
public:
  ICodeImpl();
  virtual ~ICodeImpl();
  virtual std::shared_ptr<ICodeNodeImplBase> setRoot(std::shared_ptr<ICodeNodeImplBase> node);
  virtual std::shared_ptr<ICodeNodeImplBase> getRoot() const;

private:
  std::shared_ptr<ICodeNodeImplBase> mRoot;
};

class SymbolTableEntryImpl : public SymbolTableEntryImplBase {
public:
  SymbolTableEntryImpl(const std::string &name, SymbolTableImplBase *symbol_table);
  virtual ~SymbolTableEntryImpl();
  virtual std::string name() const override;
  virtual SymbolTableImplBase *symbolTable() const override;
  virtual void appendLineNumber(int line_number) override;
  virtual std::vector<int> lineNumbers() const override;
  virtual void setAttribute(const SymbolTableKeyTypeImpl &key, const std::any &value) override;
  virtual std::any getAttribute(const SymbolTableKeyTypeImpl &key, bool *ok = nullptr) const override;
  virtual void setDefinition(const DefinitionImpl& definition) override;
  virtual DefinitionImpl getDefinition() const override;
  virtual void setTypeSpec(std::shared_ptr<TypeSpecImplBase> type_spec) override;
  virtual std::shared_ptr<TypeSpecImplBase> getTypeSpec() const override;
private:
  // TODO: do I need to use weak pointer here?
  SymbolTableImplBase *mSymbolTable;
  std::vector<int> mLineNumbers;
  std::string mName;
  std::unordered_map<SymbolTableKeyTypeImpl, std::any> mEntryMap;
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
  std::map<std::string, std::shared_ptr<SymbolTableEntryImplBase>> mSymbolMap;
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
  virtual void setProgramId(SymbolTableEntryImplBase* entry) override;
  virtual SymbolTableEntryImplBase* programId() const override;
  virtual std::shared_ptr<SymbolTableImplBase> push() override;
  virtual std::shared_ptr<SymbolTableImplBase> push(std::shared_ptr<SymbolTableImplBase> symbol_table) override;
  virtual std::shared_ptr<SymbolTableImplBase> pop() override;
private:
  // why is it necessary to maintain a standalone variable for recording the nesting level?
  // Isn't mStack.size() enough?
  int mCurrentNestingLevel;
  std::vector<std::shared_ptr<SymbolTableImplBase>> mStack;
  // entry for the main program identifier
  // TODO: do I need to use weak pointer here?
  SymbolTableEntryImplBase* mProgramId;
};

class TypeSpecImpl : public TypeSpecImplBase {
public:
  explicit TypeSpecImpl(TypeFormImpl form);
  explicit TypeSpecImpl(const std::string& value);
  virtual ~TypeSpecImpl();
  virtual TypeFormImpl form() const override;
  virtual void setIdentifier(SymbolTableEntryImplBase* identifier) override;
  virtual SymbolTableEntryImplBase* getIdentifier() const override;
  virtual void setAttribute(TypeKeyImpl key, const std::any& value) override;
  virtual std::any getAttribute(TypeKeyImpl key) const override;
  virtual bool isPascalString() const override;
  virtual TypeSpecImplBase* baseType() override;
private:
  TypeFormImpl mForm;
  SymbolTableEntryImplBase* mIdentifier;
  std::unordered_map<TypeKeyImpl, std::any> mTypeSpecMap;
};

template <>
std::unique_ptr<SymbolTableEntryImplBase>
createSymbolTableEntry(const std::string &name,
                       SymbolTableImplBase *symbolTable);

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

std::unique_ptr<TypeSpecImplBase> createStringType(const std::string& value);

#endif // INTERMEDIATEIMPL_H
