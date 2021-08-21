#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Intermediate.h"
#include <memory>
#include <map>
#include <unordered_map>

class ICodeImpl: public ICode {
public:
  ICodeImpl();
  virtual std::shared_ptr<ICodeNode> setRoot(std::shared_ptr<ICodeNode> node);
  virtual std::shared_ptr<ICodeNode> getRoot() const;
private:
  std::shared_ptr<ICodeNode> mRoot;
};

class ICodeNodeImpl: public ICodeNode {
public:
  ICodeNodeImpl(const ICodeNodeType& type);
  virtual const ICodeNode *&parent();
  virtual ICodeNodeType type() const;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node);
  virtual std::vector<std::shared_ptr<ICodeNode>> children() const;
  virtual void setAttribute(const ICodeKey& key, const QVariant& value);
  virtual QVariant getAttribute(const ICodeKey &key) const;
  virtual std::unique_ptr<ICodeNode> clone() const;
private:
  ICodeNodeType mType;
  const ICodeNode* mParent;
  std::vector<std::shared_ptr<ICodeNode>> mChildren;
  std::unordered_map<ICodeKey, QVariant> mHashTable;
};

class SymbolTableKeyImpl: public SymbolTableKey {
public:
  enum Type {
    // Constant
    CONSTANT_VALUE,
    // Procedure of function
    ROUTINE_CODE, ROUTINE_SYMTAB, ROUTINE_ICODE,
    ROUTINE_PARMS, ROUTINE_ROUTINES,
    // Variable or record field value
    DATA_VALUE
  };
  Type mType;
  virtual int type() const;
};

class SymbolTableEntryImpl: public SymbolTableEntry {
public:
  SymbolTableEntryImpl(const QString& name, SymbolTable* symbol_table);
  virtual QString name() const;
  virtual SymbolTable* symbolTable() const;
  virtual void appendLineNumber(int line_number);
  virtual QList<int> lineNumbers() const;
  virtual void setAttribute(const SymbolTableKey *key, const QVariant& value);
  virtual QVariant getAttribute(const SymbolTableKey* key, bool* ok = nullptr);
private:
  SymbolTable* mSymbolTable;
  QList<int> mLineNumbers;
  QString mName;
  std::unordered_map<int, QVariant> mEntryMap;
};

class SymbolTableImpl: public SymbolTable {
public:
  SymbolTableImpl(int nestingLevel);
  virtual int nestingLevel() const;
  virtual std::shared_ptr<SymbolTableEntry> lookup(const QString& name);
  virtual std::shared_ptr<SymbolTableEntry> enter(const QString& name);
  virtual QList<std::shared_ptr<SymbolTableEntry>> sortedEntries();
private:
  int mNestingLevel;
  std::map<QString, std::shared_ptr<SymbolTableEntry>> mSymbolMap;
};

class SymbolTableStackImpl: public SymbolTableStack {
public:
  SymbolTableStackImpl();
  virtual int currentNestingLevel() const;
  virtual std::shared_ptr<SymbolTable> localSymbolTable() const;
  virtual std::shared_ptr<SymbolTableEntry> enterLocal(const QString& name);
  virtual std::shared_ptr<SymbolTableEntry> lookupLocal(const QString& name);
  virtual std::shared_ptr<SymbolTableEntry> lookup(const QString& name);
private:
  int mCurrentNestingLevel;
  QList<std::shared_ptr<SymbolTable>> mStack;
};

#endif // INTERMEDIATEIMPL_H
