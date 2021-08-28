#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Common.h"

#include <memory>
#include <any>

template <typename SymbolTableKeyType>
class SymbolTable;

template <typename ICodeNodeType, typename ICodeKeyType>
class ICodeNode {
public:
  ICodeNode() {}
  virtual ~ICodeNode() {}
  virtual ICodeNodeType type() const = 0;
  virtual const ICodeNode*& parent() = 0;
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node) = 0;
  virtual std::vector<std::shared_ptr<ICodeNode>> children() const = 0;
  virtual void setAttribute(const ICodeKeyType& key, const std::any& value) = 0;
  virtual std::any getAttribute(const ICodeKeyType& key) const = 0;
  virtual std::unique_ptr<ICodeNode> copy() const = 0;
  virtual std::string toString() const = 0;
};

template <typename ICodeNodeType, typename ICodeKeyType>
class ICode {
public:
  ICode() {}
  virtual ~ICode() {}
  virtual std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> setRoot(std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> node) = 0;
  virtual std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> getRoot() const = 0;
};

template <typename SymbolTableKeyType>
class SymbolTableEntry {
public:
  SymbolTableEntry(const std::string& name, SymbolTable<SymbolTableKeyType>* symbol_table) {}
  virtual ~SymbolTableEntry() {}
  virtual std::string name() const = 0;
  virtual SymbolTable<SymbolTableKeyType>* symbolTable() const = 0;
  virtual void appendLineNumber(int line_number) = 0;
  virtual std::vector<int> lineNumbers() const = 0;
  virtual void setAttribute(const SymbolTableKeyType& key, const std::any& value) = 0;
  virtual std::any getAttribute(const SymbolTableKeyType& key, bool* ok = nullptr) = 0;
};

template <typename SymbolTableKeyType>
class SymbolTable {
public:
  SymbolTable(int nestingLevel) {}
  virtual ~SymbolTable() {}
  virtual int nestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>> lookup(const std::string& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>> enter(const std::string& name) = 0;
  virtual std::vector<std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>>> sortedEntries() = 0;
};

template <typename SymbolTableKeyType>
class SymbolTableStack {
public:
  SymbolTableStack() {}
  virtual ~SymbolTableStack() {}
  virtual int currentNestingLevel() const = 0;
  virtual std::shared_ptr<SymbolTable<SymbolTableKeyType>> localSymbolTable() const = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>> enterLocal(const std::string& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>> lookupLocal(const std::string& name) = 0;
  virtual std::shared_ptr<SymbolTableEntry<SymbolTableKeyType>> lookup(const std::string& name) = 0;
};

template <typename SymbolTableKeyType>
std::unique_ptr<SymbolTableEntry<SymbolTableKeyType>> createSymbolTableEntry(const std::string& name, SymbolTable<SymbolTableKeyType>* symbolTable);

template <typename SymbolTableKeyType>
std::unique_ptr<SymbolTable<SymbolTableKeyType>> createSymbolTable(int nestingLevel);

template <typename SymbolTableKeyType>
std::unique_ptr<SymbolTableStack<SymbolTableKeyType>> createSymbolTableStack();

template <typename ICodeNodeType, typename ICodeKeyType>
std::unique_ptr<ICode<ICodeNodeType, ICodeKeyType>> createICode();

template <typename ICodeNodeType, typename ICodeKeyType>
std::unique_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>> createICodeNode(const ICodeNodeType& type);

#endif // INTERMEDIATE_H
