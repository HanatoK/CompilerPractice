#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Common.h"
#include "Intermediate.h"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

typedef ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, std::unordered_map, std::vector> ICodeNodeBase;

class ICodeNodeImpl :
  public ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl, std::unordered_map, std::vector> {
public:
  ICodeNodeImpl(const ICodeNodeTypeImpl &pType);
  virtual ~ICodeNodeImpl();
  virtual ICodeNodeTypeImpl type() const;
  virtual const ICodeNodeBase* parent() const;
  virtual const ICodeNodeBase* setParent(const ICodeNodeBase* new_parent);
  virtual std::shared_ptr<ICodeNodeBase> addChild(std::shared_ptr<ICodeNodeBase> node);
  virtual void setAttribute(const ICodeKeyTypeImpl& key, const std::any& value);
  virtual std::any getAttribute(const ICodeKeyTypeImpl& key) const;
  virtual std::unique_ptr<ICodeNodeBase> copy() const;
  virtual std::string toString() const;
protected:
  virtual AttributeMapTImpl& attributeMap();
  virtual const AttributeMapTImpl& attributeMap() const;
  virtual ChildrenContainerTImpl& children();
  virtual const ChildrenContainerTImpl& children() const;
private:
  ICodeNodeTypeImpl mType;
  const ICodeNodeBase *mParent;
  AttributeMapTImpl mHashTable;
  ChildrenContainerTImpl mChildren;
};

class ICodeImpl : public ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> {
public:
  ICodeImpl();
  virtual ~ICodeImpl();
  virtual std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  setRoot(std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> node);
  virtual std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
  getRoot() const;

private:
  std::shared_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> mRoot;
};

class SymbolTableEntryImpl : public SymbolTableEntry<SymbolTableKeyTypeImpl> {
public:
  SymbolTableEntryImpl(const std::string &name,
                       SymbolTable<SymbolTableKeyTypeImpl> *symbol_table);
  virtual ~SymbolTableEntryImpl();
  virtual std::string name() const;
  virtual SymbolTable<SymbolTableKeyTypeImpl> *symbolTable() const;
  virtual void appendLineNumber(int line_number);
  virtual std::vector<int> lineNumbers() const;
  virtual void setAttribute(const SymbolTableKeyTypeImpl &key,
                            const std::any &value);
  virtual std::any getAttribute(const SymbolTableKeyTypeImpl &key,
                                bool *ok = nullptr) const;

private:
  SymbolTable<SymbolTableKeyTypeImpl> *mSymbolTable;
  std::vector<int> mLineNumbers;
  std::string mName;
  std::unordered_map<SymbolTableKeyTypeImpl, std::any> mEntryMap;
};

class SymbolTableImpl : public SymbolTable<SymbolTableKeyTypeImpl> {
public:
  SymbolTableImpl(int nestingLevel);
  virtual ~SymbolTableImpl();
  virtual int nestingLevel() const;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  lookup(const std::string &name) const;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  enter(const std::string &name);
  virtual std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>
  sortedEntries() const;

private:
  int mNestingLevel;
  std::map<std::string, std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>
      mSymbolMap;
};

class SymbolTableStackImpl : public SymbolTableStack<SymbolTableKeyTypeImpl> {
public:
  SymbolTableStackImpl();
  virtual ~SymbolTableStackImpl();
  virtual int currentNestingLevel() const;
  virtual std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>>
  localSymbolTable() const;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  enterLocal(const std::string &name);
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  lookupLocal(const std::string &name) const;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  lookup(const std::string &name) const;

private:
  int mCurrentNestingLevel;
  std::vector<std::shared_ptr<SymbolTable<SymbolTableKeyTypeImpl>>> mStack;
};

template <>
std::unique_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
createSymbolTableEntry(const std::string &name,
                       SymbolTable<SymbolTableKeyTypeImpl> *symbolTable);

template <>
std::unique_ptr<SymbolTable<SymbolTableKeyTypeImpl>>
createSymbolTable(int nestingLevel);

template <>
std::unique_ptr<SymbolTableStack<SymbolTableKeyTypeImpl>>
createSymbolTableStack();

template <>
std::unique_ptr<ICode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>> createICode();

template <>
std::unique_ptr<ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl>>
createICodeNode(const ICodeNodeTypeImpl &type);

#endif // INTERMEDIATEIMPL_H
