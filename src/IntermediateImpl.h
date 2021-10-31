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
  virtual ICodeNodeTypeImpl type() const;
  virtual const ICodeNodeImplBase* parent() const;
  virtual const ICodeNodeImplBase* setParent(const ICodeNodeImplBase* new_parent);
  virtual std::shared_ptr<ICodeNodeImplBase> addChild(std::shared_ptr<ICodeNodeImplBase> node);
  virtual void setAttribute(const ICodeKeyTypeImpl& key, const std::any& value);
  virtual std::any getAttribute(const ICodeKeyTypeImpl& key) const;
  virtual std::unique_ptr<ICodeNodeImplBase> copy() const;
  virtual std::string toString() const;
protected:
  virtual AttributeMapTImpl& attributeMap();
  virtual const AttributeMapTImpl& attributeMap() const;
  virtual ChildrenContainerTImpl& children();
  virtual const ChildrenContainerTImpl& children() const;
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
  virtual std::string name() const;
  virtual SymbolTableImplBase *symbolTable() const;
  virtual void appendLineNumber(int line_number);
  virtual std::vector<int> lineNumbers() const;
  virtual void setAttribute(const SymbolTableKeyTypeImpl &key, const std::any &value);
  virtual std::any getAttribute(const SymbolTableKeyTypeImpl &key, bool *ok = nullptr) const;
  virtual void setDefinition(const DefinitionImpl& definition);
  virtual DefinitionImpl getDefinition() const;
  virtual void setTypeSpec(std::shared_ptr<TypeSpecImplBase> type_spec);
  virtual std::shared_ptr<TypeSpecImplBase> getTypeSpec() const;
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
  virtual int nestingLevel() const;
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const;
  virtual std::shared_ptr<SymbolTableEntryImplBase> enter(const std::string &name);
  virtual std::vector<std::shared_ptr<SymbolTableEntryImplBase>> sortedEntries() const;
private:
  int mNestingLevel;
  std::map<std::string, std::shared_ptr<SymbolTableEntryImplBase>> mSymbolMap;
};

class SymbolTableStackImpl : public SymbolTableStackImplBase {
public:
  SymbolTableStackImpl();
  virtual ~SymbolTableStackImpl();
  virtual int currentNestingLevel() const;
  virtual std::shared_ptr<SymbolTableImplBase> localSymbolTable() const;
  virtual std::shared_ptr<SymbolTableEntryImplBase> enterLocal(const std::string &name);
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookupLocal(const std::string &name) const;
  virtual std::shared_ptr<SymbolTableEntryImplBase> lookup(const std::string &name) const;
  virtual void setProgramId(SymbolTableEntryImplBase* entry);
  virtual SymbolTableEntryImplBase* programId() const;
  virtual std::shared_ptr<SymbolTableImplBase> push();
  virtual std::shared_ptr<SymbolTableImplBase> push(std::shared_ptr<SymbolTableImplBase> symbol_table);
  virtual std::shared_ptr<SymbolTableImplBase> pop();
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
  virtual TypeFormImpl form() const;
  virtual void setIdentifier(SymbolTableEntryImplBase* identifier);
  virtual SymbolTableEntryImplBase* getIdentifier() const;
  virtual void setAttribute(TypeKeyImpl key, const std::any& value);
  virtual std::any getAttribute(TypeKeyImpl key) const;
  virtual bool isPascalString() const;
  TypeSpecImplBase* baseType();
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

//template <>
//std::unique_ptr<TypeSpec<SymbolTableKeyTypeImpl, DefinitionImpl, TypeFormImpl, TypeKeyImpl>> createType(const TypeFormImpl& form);

#endif // INTERMEDIATEIMPL_H
