#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include <QList>
#include <QString>
#include <QVariant>
#include <memory>
#include <QHash>

class SymbolTable;
class SymbolTableEntry;

using ICodeKey = QString;
using ICodeNodeType = QString;

class ICodeNode {
public:
  ICodeNode();
  virtual ICodeNodeType type() const = 0;
  virtual const ICodeNode*& parent() = 0;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node) = 0;
  virtual std::vector<std::shared_ptr<ICodeNode>> children() const = 0;
  virtual void setAttribute(const ICodeKey& key, const QVariant& value) = 0;
  virtual QVariant getAttribute(const ICodeKey& key) const = 0;
  virtual std::unique_ptr<ICodeNode> clone() const = 0;
};

class ICode {
public:
  ICode();
  virtual std::shared_ptr<ICodeNode> setRoot(std::shared_ptr<ICodeNode> node) = 0;
  virtual std::shared_ptr<ICodeNode> getRoot() const = 0;
};

class SymbolTableKey {
public:
  virtual int type() const = 0;
};

class SymbolTableEntry {
public:
  SymbolTableEntry(const QString& name, SymbolTable* symbol_table);
  virtual QString name() const = 0;
  virtual SymbolTable* symbolTable() const = 0;
  virtual void appendLineNumber(int line_number) = 0;
  virtual QList<int> lineNumbers() const = 0;
  virtual void setAttribute(const SymbolTableKey* key, const QVariant& value) = 0;
  virtual QVariant getAttribute(const SymbolTableKey* key, bool* ok = nullptr) = 0;
};

class SymbolTable {
public:
  SymbolTable(int nestingLevel);
  virtual int nestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTableEntry> lookup(const QString& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry> enter(const QString& name) = 0;
  virtual QList<std::shared_ptr<SymbolTableEntry>> sortedEntries() = 0;
};

class SymbolTableStack {
public:
  SymbolTableStack();
  virtual int currentNestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTable> localSymbolTable() const = 0;
  virtual std::shared_ptr<SymbolTableEntry> enterLocal(const QString& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry> lookupLocal(const QString& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry> lookup(const QString& name) = 0;
};

std::unique_ptr<SymbolTableEntry> createSymbolTableEntry(const QString& name, SymbolTable* symbolTable);
std::unique_ptr<SymbolTable> createSymbolTable(int nestingLevel);
std::unique_ptr<SymbolTableStack> createSymbolTableStack();

std::unique_ptr<ICode> createICode();
std::unique_ptr<ICodeNode> createICodeNode(const ICodeNodeType& type);

#endif // INTERMEDIATE_H
