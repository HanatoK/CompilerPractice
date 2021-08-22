#ifndef INTERMEDIATEIMPL_H
#define INTERMEDIATEIMPL_H

#include "Intermediate.h"
#include <memory>
#include <map>
#include <functional>
#include <unordered_map>

class ICodeKeyImpl;

class ICodeKeyImpl: public ICodeKey {
public:
  enum Type {
    LINE, ID, VALUE
  };
  bool operator==(const ICodeKeyImpl& rhs) const;
  bool operator!=(const ICodeKeyImpl& rhs) const;
  Type mType;
};

// this is very ugly. Probably I should use template
class ICodeNodeTypeImpl: public ICodeNodeType {
public:
  enum Type {
    // program structures
    PROGRAM, PROCEDURE, FUNCTION,
    // statements
    COMPOUND, ASSIGN, LOOP, TEST, CALL, PARAMETERS,
    IF, SELECT, SELECT_BRANCH, SELECT_CONSTANTS, NO_OP,
    // relational operators
    EQ, NE, LT, LE, GT, GE, NOT,
    // additive operators
    ADD, SUBTRACT, OR, NEGATE,
    // multiplicative operatros
    MULTIPLY, INTEGER_DIVIDE, FLOAT_DIVIDE, MOD, AND,
    // operands
    VARIABLE, SUBSCRIPTS, FIELD,
    INTEGER_CONSTANT, REAL_CONSTANT,
    STRING_CONSTANT, BOOLEAN_CONSTANT
  };
  ICodeNodeTypeImpl() {}
  ICodeNodeTypeImpl(Type t): mType(t) {}
  ICodeNodeTypeImpl(const ICodeNodeTypeImpl& rhs): mType(rhs.mType) {}
  ICodeNodeTypeImpl& operator=(const ICodeNodeTypeImpl& rhs) {
    if (this == &rhs) {
      return *this;
    }
    this->mType = rhs.mType;
    return *this;
  }
  Type mType;
  bool operator==(const ICodeNodeTypeImpl& rhs) const;
  bool operator!=(const ICodeNodeTypeImpl& rhs) const;
};

namespace std {
template <>
struct hash<ICodeKeyImpl> {
  std::size_t operator()(const ICodeKeyImpl& k) const {
    return std::hash<int>()(int(k.mType));
  }
};

template<>
struct hash<ICodeNodeTypeImpl> {
  std::size_t operator()(const ICodeNodeTypeImpl& k) const {
    return std::hash<int>()(int(k.mType));
  }
};
}

class ICodeNodeImpl: public ICodeNode {
public:
  ICodeNodeImpl(const ICodeNodeType* pType);
  virtual const ICodeNode *&parent();
  virtual std::unique_ptr<ICodeNodeType> type() const;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node);
  virtual std::vector<std::shared_ptr<ICodeNode>> children() const;
  virtual void setAttribute(const ICodeKey* key, const QVariant& value);
  virtual QVariant getAttribute(const ICodeKey* key) const;
  // only copy this node itself, not the parent and children!
  virtual std::unique_ptr<ICodeNode> copy() const;
  virtual QString toString() const;
private:
  ICodeNodeTypeImpl mType;
  const ICodeNode* mParent;
  std::vector<std::shared_ptr<ICodeNode>> mChildren;
  std::unordered_map<ICodeKeyImpl, QVariant> mHashTable;
};

class ICodeImpl: public ICode {
public:
  ICodeImpl();
  virtual std::shared_ptr<ICodeNode> setRoot(std::shared_ptr<ICodeNode> node);
  virtual std::shared_ptr<ICodeNode> getRoot() const;
private:
  std::shared_ptr<ICodeNode> mRoot;
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
  bool operator==(const SymbolTableKeyImpl& rhs) const;
  bool operator!=(const SymbolTableKeyImpl& rhs) const;
  Type mType;
};

namespace std {
template <>
struct hash<SymbolTableKeyImpl> {
  std::size_t operator()(const SymbolTableKeyImpl& k) const {
    return std::hash<int>()(int(k.mType));
  }
};
}

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
  std::unordered_map<SymbolTableKeyImpl, QVariant> mEntryMap;
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
