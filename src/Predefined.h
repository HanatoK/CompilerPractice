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
  // Pascal's standard procedures and functions in listing 11-10
  std::shared_ptr<SymbolTableEntryImplBase> readId;
  std::shared_ptr<SymbolTableEntryImplBase> readlnId;
  std::shared_ptr<SymbolTableEntryImplBase> writeId;
  std::shared_ptr<SymbolTableEntryImplBase> writelnId;
  std::shared_ptr<SymbolTableEntryImplBase> absId;
  std::shared_ptr<SymbolTableEntryImplBase> arctanId;
  std::shared_ptr<SymbolTableEntryImplBase> chrId;
  std::shared_ptr<SymbolTableEntryImplBase> cosId;
  std::shared_ptr<SymbolTableEntryImplBase> eofId;
  std::shared_ptr<SymbolTableEntryImplBase> eolnId;
  std::shared_ptr<SymbolTableEntryImplBase> expId;
  std::shared_ptr<SymbolTableEntryImplBase> lnId;
  std::shared_ptr<SymbolTableEntryImplBase> oddId;
  std::shared_ptr<SymbolTableEntryImplBase> ordId;
  std::shared_ptr<SymbolTableEntryImplBase> predId;
  std::shared_ptr<SymbolTableEntryImplBase> roundId;
  std::shared_ptr<SymbolTableEntryImplBase> sinId;
  std::shared_ptr<SymbolTableEntryImplBase> sqrId;
  std::shared_ptr<SymbolTableEntryImplBase> sqrtId;
  std::shared_ptr<SymbolTableEntryImplBase> succId;
  std::shared_ptr<SymbolTableEntryImplBase> truncId;
  inline static Predefined* _instance = nullptr;
public:
  Predefined(Predefined const&) = delete;
  void operator=(Predefined const&) = delete;
  static Predefined& instance(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  // this overload can only be called after initialization!
  static Predefined& instance();
private:
  void initialize(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  void initializeTypes(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  void initializeConstants(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  void initializeStandardRoutines(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack);
  static std::shared_ptr<SymbolTableEntryImplBase> enterStandard(
    std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack,
    DefinitionImpl definition, const std::string& name,
    RoutineCodeImpl routine_code);
  Predefined();
};

#endif // PREDEFINED_H
