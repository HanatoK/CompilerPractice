#ifndef PREDEFINED_H
#define PREDEFINED_H

#include "Intermediate.h"

class Predefined {
public:
  // predefined types
  std::shared_ptr<TypeSpecImplBase> integerType;
  std::shared_ptr<TypeSpecImplBase> realType;
  std::shared_ptr<TypeSpecImplBase> booleanType;
  std::shared_ptr<TypeSpecImplBase> charType;
  std::shared_ptr<TypeSpecImplBase> undefinedType;
  // predefined identifiers
  std::shared_ptr<SymbolTableEntryImplBase> integerId;
  std::shared_ptr<SymbolTableEntryImplBase> realId;
  std::shared_ptr<SymbolTableEntryImplBase> booleanId;
  std::shared_ptr<SymbolTableEntryImplBase> charId;
  std::shared_ptr<SymbolTableEntryImplBase> falseId;
  std::shared_ptr<SymbolTableEntryImplBase> trueId;
  inline static Predefined* _instance = nullptr;
public:
  Predefined(Predefined const&) = delete;
  void operator=(Predefined const&) = delete;
  static Predefined& instance(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  // this overload can only be called after initialization!
  static Predefined& instance();
private:
  void initialize(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  Predefined();
};

#endif // PREDEFINED_H
