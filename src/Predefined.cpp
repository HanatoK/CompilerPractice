#include "Predefined.h"
#include "IntermediateImpl.h"
#include <stdexcept>

Predefined& Predefined::instance(std::shared_ptr<SymbolTableStackImplBase>& symbol_table_stack)
{
  static Predefined s;
  static bool firsttime = true;
  if (firsttime) {
    s.initialize(symbol_table_stack);
    _instance = &s;
    firsttime = false;
  }
  return s;
}

Predefined& Predefined::instance()
{
  if (_instance == nullptr) {
    throw std::runtime_error("Uninitialized singleton of Predefined!");
  }
  return *_instance;
}

Predefined::Predefined()
{
}

void Predefined::initialize(std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack) {
  initializeTypes(symbol_table_stack);
  initializeConstants(symbol_table_stack);
  initializeStandardRoutines(symbol_table_stack);
}

void Predefined::initializeTypes(std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack) {
  // integer type
  integerType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  integerId = symbol_table_stack->enterLocal("integer");
  integerId->setDefinition(DefinitionImpl::TYPE);
  integerId->setTypeSpec(integerType);
  integerType->setIdentifier(integerId);
  // real type
  realType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  realId = symbol_table_stack->enterLocal("real");
  realId->setDefinition(DefinitionImpl::TYPE);
  realId->setTypeSpec(realType);
  realType->setIdentifier(realId);
  // boolean type
  booleanType = std::make_unique<TypeSpecImpl>(TypeFormImpl::ENUMERATION);
  booleanId = symbol_table_stack->enterLocal("boolean");
  booleanId->setDefinition(DefinitionImpl::TYPE);
  booleanId->setTypeSpec(booleanType);
  booleanType->setIdentifier(booleanId);
  // char type
  charType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
  charId = symbol_table_stack->enterLocal("char");
  charId->setDefinition(DefinitionImpl::TYPE);
  charId->setTypeSpec(charType);
  charType->setIdentifier(charId);
  // undefined type
  undefinedType = std::make_unique<TypeSpecImpl>(TypeFormImpl::SCALAR);
}

void Predefined::initializeConstants(std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack) {
  // boolean enumeration constant false
  falseId = symbol_table_stack->enterLocal("false");
  falseId->setDefinition(DefinitionImpl::ENUMERATION_CONSTANT);
  falseId->setTypeSpec(booleanType);
  falseId->setAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>(VariableValueT(0ll));
  // boolean enumeration constant true
  trueId= symbol_table_stack->enterLocal("true");
  trueId->setDefinition(DefinitionImpl::ENUMERATION_CONSTANT);
  trueId->setTypeSpec(booleanType);
  trueId->setAttribute<SymbolTableKeyTypeImpl::CONSTANT_VALUE>(VariableValueT(1ll));
  std::vector<std::weak_ptr<SymbolTableEntryImplBase>> constants;
  constants.push_back(falseId);
  constants.push_back(trueId);
  booleanType->setAttribute<TypeKeyImpl::ENUMERATION_CONSTANTS>(constants);
}

void Predefined::initializeStandardRoutines(
    std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack) {
#define ENTER_PROCEDURE(name) {using enum RoutineCodeImpl; name ## Id = enterStandard(symbol_table_stack, DefinitionImpl::PROCEDURE, #name, name);}
  ENTER_PROCEDURE(read)
  ENTER_PROCEDURE(readln)
  ENTER_PROCEDURE(write)
  ENTER_PROCEDURE(writeln)
#undef ENTER_PROCEDURE

#define ENTER_FUNCTION(name) {using enum RoutineCodeImpl; name ## Id = enterStandard(symbol_table_stack, DefinitionImpl::FUNCTION, #name, name);}
  ENTER_FUNCTION(abs)
  ENTER_FUNCTION(arctan)
  ENTER_FUNCTION(chr)
  ENTER_FUNCTION(cos)
  ENTER_FUNCTION(eof)
  ENTER_FUNCTION(eoln)
  ENTER_FUNCTION(exp)
  ENTER_FUNCTION(ln)
  ENTER_FUNCTION(odd)
  ENTER_FUNCTION(ord)
  ENTER_FUNCTION(pred)
  ENTER_FUNCTION(round)
  ENTER_FUNCTION(sin)
  ENTER_FUNCTION(sqr)
  ENTER_FUNCTION(sqrt)
  ENTER_FUNCTION(succ)
  ENTER_FUNCTION(trunc)
#undef ENTER_FUNCTION
}

std::shared_ptr<SymbolTableEntryImplBase>
Predefined::enterStandard(
    std::shared_ptr<SymbolTableStackImplBase> &symbol_table_stack,
    DefinitionImpl definition, const std::string &name,
    RoutineCodeImpl routine_code) {
  auto proc_id = symbol_table_stack->enterLocal(name);
  proc_id->setDefinition(definition);
  proc_id->setAttribute<SymbolTableKeyTypeImpl::ROUTINE_CODE>(routine_code);
  return proc_id;
}
