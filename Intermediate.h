#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Common.h"

#include <memory>
#include <any>
#include <vector>
#include <unordered_map>

template <typename SymbolTableKeyType>
class SymbolTable;

template <typename ICodeNodeType, typename ICodeKeyType>
class ICodeNode {
public:
  ICodeNode() {}
  ICodeNode(const ICodeNodeType& pType): mType(pType), mParent(nullptr) {}
  virtual ~ICodeNode() {}
  virtual ICodeNodeType type() const {
    return mType;
  }
  virtual const ICodeNode*& parent() {
    return mParent;
  }
  virtual std::shared_ptr<ICodeNode> addChild(std::shared_ptr<ICodeNode> node) {
    if (node != nullptr) {
      mChildren.push_back(node);
      node->parent() = dynamic_cast<const ICodeNode*>(this);
    }
    return node;
  }
  virtual std::vector<std::shared_ptr<ICodeNode>> children() {
    return mChildren;
  }
  virtual void setAttribute(const ICodeKeyType& key, const std::any& value) {
    mHashTable[key] = value;
  }
  virtual std::any getAttribute(const ICodeKeyType& key) const {
    const auto search = mHashTable.find(key);
    if (search != mHashTable.end()) {
      return search->second;
    } else {
      return std::any();
    }
  }
  virtual std::unique_ptr<ICodeNode> copy() const {
    // only copy this node itself, not the parent and children!
    auto new_node = createICodeNode<ICodeNodeType, ICodeKeyType>(this->mType);
    for (auto it = mHashTable.begin(); it != mHashTable.end(); ++it) {
      new_node->mHashTable[it->first] = it->second;
    }
    return std::move(new_node);
  }
  virtual std::string toString() const = 0;
  const std::unordered_map<ICodeKeyTypeImpl, std::any>& attributeTable() const {
    return mHashTable;
  }
protected:
  ICodeNodeType mType;
  const ICodeNode<ICodeNodeType, ICodeKeyType> *mParent;
  std::unordered_map<ICodeKeyType, std::any> mHashTable;
  std::vector<std::shared_ptr<ICodeNode<ICodeNodeType, ICodeKeyType>>> mChildren;
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
