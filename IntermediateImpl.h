#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Common.h"
#include "Intermediate.h"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

class ICodeNodeImpl : public ICodeNode<ICodeNodeTypeImpl, ICodeKeyTypeImpl> {
public:
  ICodeNodeImpl(const ICodeNodeTypeImpl &pType);
  virtual ~ICodeNodeImpl();
  virtual const ICodeNode *&parent();
  virtual ICodeNodeTypeImpl type() const;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node);
  virtual std::vector<std::shared_ptr<ICodeNode>> children() const;
  virtual void setAttribute(const ICodeKeyTypeImpl &key, const std::any &value);
  virtual std::any getAttribute(const ICodeKeyTypeImpl &key) const;
  // only copy this node itself, not the parent and children!
  virtual std::unique_ptr<ICodeNode> copy() const;
  virtual std::string toString() const;

private:
  ICodeNodeTypeImpl mType;
  const ICodeNode *mParent;
  std::vector<std::shared_ptr<ICodeNode>> mChildren;
  std::unordered_map<ICodeKeyTypeImpl, std::any> mHashTable;
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
                                bool *ok = nullptr);

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
  lookup(const std::string &name);
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  enter(const std::string &name);
  virtual std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>>
  sortedEntries();

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
  lookupLocal(const std::string &name);
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyTypeImpl>>
  lookup(const std::string &name);

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
